const std = @import("std");

pub fn find(comptime T: type, slice: []const T, target: *const T) ?usize {
    return for (slice, 0..) |*item, index| {
        if (std.mem.eql(T, item[0..1], target[0..1])) {
            break index;
        }
    } else null;
}
