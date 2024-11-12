// Program 3: Prace s retezci a vestavenymi funkcemi
const ifj = @import("ifj24.zig");
// Hlavni funkce
pub fn main() void {
const str1 = ifj.string("Toto je text v programu jazyka IFJ24");
var str2 = ifj.string(", ktery jeste trochu obohatime");
str2 = ifj.concat(str1, str2);
ifj. write(str1); ifj . write("\n");
ifj .write(str2); ifj.write("\n");
ifj.write("Zadejte serazenou posloupnost malych pismen a-h:\n");
var newInput : i32 = 3;
while (newInput > 0) {
ifj.write(newInput);
ifj.write("\n");
newInput = newInput - 1; // misto break;
}
}
