const std = @import("std");
const Wallbounce = @import("wallbounce.zig").Wallbounce;

pub fn main(init: std.process.Init) !void {
    _ = init;
    var wallbounce = Wallbounce.init(std.heap.c_allocator);
    defer wallbounce.deinit();

    try wallbounce.run();
}
