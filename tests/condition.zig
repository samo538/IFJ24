const ifj = @import("ifj24.zig");
pub fn main() void {
var ct : i32 = 12 + 5;
if(12 < ct) {
ifj.write("<\n");
}
else {ifj.write("error\n");}
if(ct > 12) {
ifj.write(">\n");
}
else {ifj.write("error\n");}
if(ct <= 17) {
ifj.write("<=\n");
}
else {ifj.write("error\n");}
if(12 <= ct) {
ifj.write("<=\n");
}
else {ifj.write("error\n");}
if(17 >= ct) {
ifj.write(">=\n");
}
else {ifj.write("error\n");}
if(ct >= 12) {
ifj.write(">=\n");
}
else {ifj.write("error\n");}
if(ct == 17) {
ifj.write("==\n");
}
else {ifj.write("error\n");}
if(ct != 12) {
ifj.write("!=\n");
}
else {ifj.write("error\n");}
ifj.write(ct);
}
