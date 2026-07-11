const rl = @import("raylib");

pub const GameObjectType = enum {
    lava,
    shield,
};

pub const GameObject = struct {
    type: GameObjectType,
    rect: rl.Rectangle,
    decayed: bool,
    has_collision: bool,

    pub fn init() @This() {
        return .{
            .rect = .{
                .x = 0,
                .y = 0,
                .width = 0,
                .height = 0,
            },
            .type = .lava,
            .decayed = false,
        };
    }
};
