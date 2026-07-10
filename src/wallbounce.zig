const GameState = @import("game_state.zig").GameState;
const GameObject = @import("game_object.zig").GameObject;
const rl = @import("raylib");
const std = @import("std");

const fixedTimestep = 1.0 / 60.0;

const Wall = enum { left, right };

pub const Wallbounce = struct {
    state: GameState,
    delta: f32,
    accumulator: f32,
    screenWidth: f32,
    screenHeight: f32,
    leftWallObjects: std.ArrayList(GameObject),
    rightWallObjects: std.ArrayList(GameObject),
    score: u32,
    highScore: u32,
    playerRectangle: rl.Rectangle,
    playerVelocity: rl.Vector2,
    shields: u32,
    prng: std.Random.DefaultPrng,
    gravity: f32,
    horizontalSpeed: f32,
    wallLeft: rl.Rectangle,
    wallRight: rl.Rectangle,
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) @This() {
        const self = @This(){
            .state = .main,
            .delta = 1.0 / 60.0,
            .accumulator = 0.0,
            .screenWidth = 0.0,
            .screenHeight = 0.0,
            .leftWallObjects = .empty,
            .rightWallObjects = .empty,
            .score = 0,
            .highScore = 0,
            .playerRectangle = .{
                .x = 0,
                .y = 0,
                .width = 0,
                .height = 0,
            },
            .playerVelocity = .{
                .x = 1.0,
                .y = 0.0,
            },
            .shields = 0,
            .prng = std.Random.DefaultPrng.init(0),
            .gravity = 25.0,
            .horizontalSpeed = 200.0,
            .wallLeft = .{
                .x = 0,
                .y = 0,
                .width = 20.0,
                .height = 0,
            },
            .wallRight = .{
                .x = 0,
                .y = 0,
                .width = 20.0,
                .height = 0,
            },
            .allocator = allocator,
        };
        return self;
    }

    pub fn deinit(self: *@This()) void {
        self.leftWallObjects.deinit(self.allocator);
        self.rightWallObjects.deinit(self.allocator);
    }

    pub fn run(self: *@This()) error{OutOfMemory}!void {
        rl.initWindow(500, 700, "wallbounce");
        defer rl.closeWindow();

        while (!rl.windowShouldClose()) {
            self.screenWidth = @floatFromInt(rl.getScreenWidth());
            self.screenHeight = @floatFromInt(rl.getScreenHeight());
            if (@mod(self.screenHeight, 50) != 0) {
                std.debug.print("Screen height is not divisible by 50", .{});
                break;
            }
            self.wallLeft.height = self.screenHeight;
            self.wallRight.x = self.screenWidth - self.wallRight.width;
            self.wallRight.height = self.screenHeight;

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
                GameState.main => self.mainStateFrame(),
                GameState.playing => self.playingStateFrame(),
                GameState.stop => self.stopStateFrame(),
            }
        }
    }

    fn mainStateFrame(self: *@This()) void {
        const startButtonWidth = 150.0;
        const startButtonHeight = 50.0;
        const startButton = rl.Rectangle{
            .width = startButtonWidth,
            .height = startButtonHeight,
            .x = self.screenWidth / 2.0 - startButtonWidth / 2.0,
            .y = self.screenHeight - startButtonHeight - 150.0,
        };

        rl.drawRectangleRec(startButton, .gray);

        if (rl.isMouseButtonPressed(.left)) {
            if (rl.checkCollisionPointRec(rl.getMousePosition(), startButton)) {
                self.state = .playing;

                // Reset state
                self.leftWallObjects.clearRetainingCapacity();
                self.rightWallObjects.clearRetainingCapacity();

                self.score = 0;

                self.playerRectangle = .{
                    .x = 0,
                    .y = 0,
                    .width = 50,
                    .height = 50,
                };

                self.playerVelocity = .{
                    .x = 1.0,
                    .y = 0.0,
                };

                self.shields = 0;
            }
        }
    }

    fn stopStateFrame(self: *@This()) void {
        _ = self;
    }

    fn playingStateFrame(self: *@This()) void {
        rl.drawRectangleRec(self.playerRectangle, .light_gray);
    }

    fn updatePhysics(self: *@This()) error{OutOfMemory}!void {
        if (self.state != .playing) return;

        self.playerVelocity.y += self.gravity * fixedTimestep;

        self.playerRectangle.y += self.playerVelocity.y;

        // Move player horizontally
        self.playerRectangle.x += self.horizontalSpeed * self.playerVelocity.x * fixedTimestep;

        // Don't go above the ceiling
        if (self.playerRectangle.y < 0) {
            self.playerRectangle.y = 0;
            self.playerVelocity.y = self.gravity * fixedTimestep;
        }

        // Teleport to the top when reached the bottom
        if (self.playerRectangle.y > self.screenHeight) {
            self.playerRectangle.y = 0;
        }

        if (rl.checkCollisionRecs(self.playerRectangle, self.wallLeft) or rl.checkCollisionRecs(self.playerRectangle, self.wallRight)) {
            // Bounce off a wall
            self.playerVelocity.x = -self.playerVelocity.x;

            // Update the score
            self.score += 1;
            if (self.score > self.highScore) {
                self.highScore = self.score;
            }

            // Generate a wall opposite to the player
            try if (self.playerVelocity.x < 0)
                self.generate(.left)
            else
                self.generate(.right);
        }
    }

    fn generate(self: *@This(), wall: Wall) error{OutOfMemory}!void {
        switch (wall) {
            .left => self.leftWallObjects.clearRetainingCapacity(),
            .right => self.rightWallObjects.clearRetainingCapacity(),
        }

        const tiles: usize = @intFromFloat(self.screenHeight / 50.0);
        _ = tiles;

        var skip = std.ArrayList(usize).empty;
        defer skip.deinit(self.allocator);
        // Generate an empty space
        {
            const y = self.prng.random().uintAtMost(usize, @intFromFloat(self.screenHeight / 50 - 2));
            try skip.append(self.allocator, y);
            try skip.append(self.allocator, y + 1);
            try skip.append(self.allocator, y + 2);
        }

        // Generate a shield
        if (self.prng.random().uintAtMost(usize, 10) == 1 and false) {
            const shieldPos = self.prng.random().uintAtMost(usize, @as(usize, @intFromFloat(self.screenWidth / 50)) - 2);
            while (true) {
                for (skip.items) |i| {
                    _ = i;
                }
            }
            _ = shieldPos;
        }
    }
};
