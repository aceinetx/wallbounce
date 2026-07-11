const GameState = @import("game_state.zig").GameState;
const GameObject = @import("game_object.zig").GameObject;
const util = @import("util.zig");
const rl = @import("raylib");
const std = @import("std");

const fixedTimestep = 1.0 / 60.0;

const Wall = enum { left, right };

pub const Wallbounce = struct {
    state: GameState,
    delta: f32,
    accumulator: f32,
    screen_width: f32,
    screen_height: f32,
    left_wall_objects: std.ArrayList(GameObject),
    right_wall_objects: std.ArrayList(GameObject),
    score: u32,
    high_score: u32,
    player_rectangle: rl.Rectangle,
    player_velocity: rl.Vector2,
    shields: u32,
    prng: std.Random.DefaultPrng,
    gravity: f32,
    horizontal_speed: f32,
    jump_velocity: f32,
    wall_left: rl.Rectangle,
    wall_right: rl.Rectangle,
    shield_texture: rl.Texture,
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) @This() {
        const self = @This(){
            .state = .main,
            .delta = 1.0 / 60.0,
            .accumulator = 0.0,
            .screen_width = 0.0,
            .screen_height = 0.0,
            .left_wall_objects = .empty,
            .right_wall_objects = .empty,
            .score = 0,
            .high_score = 0,
            .player_rectangle = .{
                .x = 0,
                .y = 0,
                .width = 0,
                .height = 0,
            },
            .player_velocity = .{
                .x = 1.0,
                .y = 0.0,
            },
            .shields = 0,
            .prng = std.Random.DefaultPrng.init(0),
            .gravity = 25.0,
            .horizontal_speed = 200.0,
            .jump_velocity = -500.0,
            .wall_left = .{
                .x = 0,
                .y = 0,
                .width = 20.0,
                .height = 0,
            },
            .wall_right = .{
                .x = 0,
                .y = 0,
                .width = 20.0,
                .height = 0,
            },
            .shield_texture = undefined,
            .allocator = allocator,
        };
        return self;
    }

    pub fn deinit(self: *@This()) void {
        self.left_wall_objects.deinit(self.allocator);
        self.right_wall_objects.deinit(self.allocator);
    }

    pub fn run(self: *@This()) error{ OutOfMemory, LoadTexture }!void {
        rl.initWindow(500, 700, "wallbounce");
        defer rl.closeWindow();

        self.shield_texture = try rl.loadTexture("assets/shield.png");
        defer self.shield_texture.unload();

        while (!rl.windowShouldClose()) {
            self.screen_width = @floatFromInt(rl.getScreenWidth());
            self.screen_height = @floatFromInt(rl.getScreenHeight());
            if (@mod(self.screen_height, 50) != 0) {
                std.debug.print("Screen height is not divisible by 50", .{});
                break;
            }
            self.wall_left.height = self.screen_height;
            self.wall_right.x = self.screen_width - self.wall_right.width;
            self.wall_right.height = self.screen_height;

            self.delta = rl.getFrameTime();

            if (self.state == .playing) {
                self.accumulator += self.delta;

                while (self.accumulator >= fixedTimestep) {
                    try self.updatePhysics();
                    self.accumulator -= fixedTimestep;
                }
            }

            rl.beginDrawing();
            defer rl.endDrawing();

            rl.clearBackground(.white);

            switch (self.state) {
                GameState.main => try self.mainStateFrame(),
                GameState.playing => self.playingStateFrame(),
                GameState.stop => try self.mainStateFrame(),
            }
        }
    }

    fn isJumpButtonPressed(self: *const @This()) bool {
        _ = self;
        return rl.isKeyPressed(rl.KeyboardKey.space) or
            rl.isMouseButtonPressed(rl.MouseButton.left) or rl.isKeyPressed(rl.KeyboardKey.up);
    }

    fn mainStateFrame(self: *@This()) std.mem.Allocator.Error!void {
        const startButtonWidth = 150.0;
        const startButtonHeight = 50.0;
        const start_button = rl.Rectangle{
            .width = startButtonWidth,
            .height = startButtonHeight,
            .x = self.screen_width / 2.0 - startButtonWidth / 2.0,
            .y = self.screen_height - startButtonHeight - 150.0,
        };

        rl.drawRectangleRec(start_button, .gray);
        rl.drawText(
            "Start",
            @intFromFloat(start_button.x + start_button.width / 8.0),
            @intFromFloat(start_button.y + start_button.height / 8.0),
            40.0,
            .white,
        );

        if (self.state == .stop) {
            rl.drawText(rl.textFormat("High score: %d", .{self.high_score}), 0, 0, 25, .light_gray);
        }

        if ((rl.isMouseButtonPressed(.left) and rl.checkCollisionPointRec(rl.getMousePosition(), start_button)) or self.isJumpButtonPressed()) {
            self.state = .playing;

            // Reset state
            self.left_wall_objects.clearRetainingCapacity();
            self.right_wall_objects.clearRetainingCapacity();

            self.score = 0;

            self.player_rectangle = .{
                .x = self.screen_width / 2.0,
                .y = self.screen_height / 2.0,
                .width = 50,
                .height = 50,
            };
            self.player_rectangle.x -= self.player_rectangle.width / 2.0;
            self.player_rectangle.y -= self.player_rectangle.height / 2.0;

            self.player_velocity = .{
                .x = 1.0,
                .y = 0.0,
            };

            self.shields = 0;

            try self.generate(.left);
            try self.generate(.right);
        }
    }

    fn stopStateFrame(self: *@This()) void {
        self.state = .main;
    }

    fn drawObjects(self: *@This(), objects: *const std.ArrayList(GameObject)) void {
        for (objects.items) |*obj| {
            if (obj.decayed) {
                continue;
            }

            switch (obj.type) {
                .lava => {
                    rl.drawRectangleRec(obj.rect, .red);
                },
                .shield => {
                    rl.drawTextureEx(self.shield_texture, .{
                        .x = obj.rect.x,
                        .y = obj.rect.y,
                    }, 0.0, 0.05, .white);
                },
            }
        }
    }

    fn playingStateFrame(self: *@This()) void {
        rl.drawRectangleRec(self.player_rectangle, .light_gray);
        if (self.shields > 0)
            rl.drawRectangleLinesEx(self.player_rectangle, 3.0, .blue);

        rl.drawRectangleRec(self.wall_left, .gray);
        rl.drawRectangleRec(self.wall_right, .gray);

        if (self.isJumpButtonPressed()) {
            // Jump
            self.player_velocity.y = self.jump_velocity * fixedTimestep;
        }

        self.drawObjects(&self.left_wall_objects);
        self.drawObjects(&self.right_wall_objects);

        {
            const text = rl.textFormat("%d", .{self.score});
            const fontSize = 50.0;
            const width: f32 = @floatFromInt(rl.measureText(text, fontSize));
            rl.drawText(text, @intFromFloat(self.screen_width / 2.0 - width / 2.0), 0, fontSize, .light_gray);
        }
        {
            const text = rl.textFormat("%d", .{self.shields});
            const fontSize = 50.0;
            const width: f32 = @floatFromInt(rl.measureText(text, fontSize));
            rl.drawText(text, 35, 0, fontSize, .light_gray);
            rl.drawTextureEx(self.shield_texture, .{ .x = 40 + width, .y = 3 }, 0.0, 0.05, .light_gray);
        }
    }

    fn updatePhysicsOnObjects(self: *@This(), objects: *const std.ArrayList(GameObject)) void {
        var shield_used = false;
        for (objects.items) |*obj| {
            if (obj.decayed) continue;
            if (!obj.has_collision) continue;

            if (rl.checkCollisionRecs(self.player_rectangle, obj.rect)) {
                switch (obj.type) {
                    .lava => {
                        if (self.shields == 0) {
                            self.state = .stop;
                        } else {
                            shield_used = true;
                            for (objects.items) |*i| i.has_collision = false;
                        }
                    },
                    .shield => {
                        self.shields += 1;
                        obj.decayed = true;
                    },
                }
            }
        }

        if (shield_used) {
            self.shields -= 1;
        }
    }

    fn updatePhysics(self: *@This()) error{OutOfMemory}!void {
        if (self.state != .playing) return;

        self.player_velocity.y += self.gravity * fixedTimestep;

        self.player_rectangle.y += self.player_velocity.y;

        // Move player horizontally
        self.player_rectangle.x += self.horizontal_speed * self.player_velocity.x * fixedTimestep;

        // Don't go above the ceiling
        if (self.player_rectangle.y < 0) {
            self.player_rectangle.y = 0;
            self.player_velocity.y = self.gravity * fixedTimestep;
        }

        // Teleport to the top when reached the bottom
        if (self.player_rectangle.y > self.screen_height) {
            self.player_rectangle.y = 0;
        }

        if (rl.checkCollisionRecs(self.player_rectangle, self.wall_left) or rl.checkCollisionRecs(self.player_rectangle, self.wall_right)) {
            // Bounce off a wall
            self.player_velocity.x = -self.player_velocity.x;

            // Update the score
            self.score += 1;
            if (self.score > self.high_score) {
                self.high_score = self.score;
            }

            // Generate a wall opposite to the player
            try if (self.player_velocity.x < 0)
                self.generate(.left)
            else
                self.generate(.right);
        }

        updatePhysicsOnObjects(self, &self.left_wall_objects);
        updatePhysicsOnObjects(self, &self.right_wall_objects);
    }

    fn generate(self: *@This(), wall: Wall) std.mem.Allocator.Error!void {
        switch (wall) {
            .left => self.left_wall_objects.clearRetainingCapacity(),
            .right => self.right_wall_objects.clearRetainingCapacity(),
        }

        const tiles: usize = @intFromFloat(self.screen_height / 50.0);

        var skip = std.ArrayList(usize).empty;
        defer skip.deinit(self.allocator);
        // Generate an empty space
        {
            const y = self.prng.random().uintAtMost(usize, @intFromFloat(self.screen_height / 50 - 2));
            try skip.append(self.allocator, y);
            try skip.append(self.allocator, y + 1);
            try skip.append(self.allocator, y + 2);
        }

        // Generate a shield
        if (self.prng.random().uintAtMost(usize, 10) == 1) {
            var shield_pos: usize = 0;
            while (true) {
                shield_pos = self.prng.random().uintAtMost(usize, @as(usize, @intFromFloat(self.screen_width / 50)) - 2);
                var invalid = false;
                for (skip.items) |i| {
                    if (i -% 1 == shield_pos or i -% 2 == shield_pos or i == shield_pos or i +% 1 == shield_pos or i +% 2 == shield_pos) {
                        invalid = true;
                        break;
                    }
                }
                if (!invalid) break;
            }

            try skip.append(self.allocator, shield_pos);
            try skip.append(self.allocator, shield_pos + 1);

            const shield_pos_float: f32 = @floatFromInt(shield_pos);

            var obj = GameObject{
                .type = .shield,
                .rect = .{
                    .x = 0,
                    .y = 50.0 * shield_pos_float + 25.0,
                    .width = 10.0,
                    .height = 50.0,
                },
                .decayed = false,
                .has_collision = true,
            };
            switch (wall) {
                .left => {
                    obj.rect.x = 20;
                    obj.rect.width = 10;
                    try self.left_wall_objects.append(self.allocator, obj);
                },
                .right => {
                    obj.rect.x = self.screen_width - 50;
                    try self.right_wall_objects.append(self.allocator, obj);
                },
            }
        }

        // Generate lava
        for (0..tiles) |i| {
            if (util.find(usize, skip.items, &i) != null) continue;

            var obj = GameObject{
                .type = .lava,
                .rect = .{
                    .x = 0,
                    .y = 50.0 * @as(f32, @floatFromInt(i)),
                    .width = 10.0,
                    .height = 50.0,
                },
                .decayed = false,
                .has_collision = true,
            };

            switch (wall) {
                .left => {
                    obj.rect.x = 20;
                    obj.rect.width = 10;
                    try self.left_wall_objects.append(self.allocator, obj);
                },
                .right => {
                    obj.rect.x = self.screen_width - 30;
                    try self.right_wall_objects.append(self.allocator, obj);
                },
            }
        }
    }
};
