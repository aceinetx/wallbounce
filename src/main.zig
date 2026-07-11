const std = @import("std");
const Wallbounce = @import("wallbounce.zig").Wallbounce;

pub fn main(init: std.process.Init) !void {
    _ = init;
    var gpa: std.heap.DebugAllocator(.{}) = .init;
    defer _ = gpa.deinit();

    var wallbounce = Wallbounce.init(gpa.allocator());
    defer wallbounce.deinit();

    try wallbounce.run();
}
