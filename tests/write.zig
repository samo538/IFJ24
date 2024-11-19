const ifj = @import("ifj24.zig");
pub fn main() void {
ifj.write("Test write\n");
ifj.write(12);
ifj.write("\n");
ifj.write(12.22);
ifj.write("\n");
}
