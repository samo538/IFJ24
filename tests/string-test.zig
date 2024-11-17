const ifj = @import("ifj24.zig");

// Hlavni funkce
pub fn main() void {
    
    const str1 = ifj.string("abcdefgh");
    const str2 = ifj.string("ABCDEFGH");
    const str3 = ifj.string("!@#$%^&*()_+~-={}[]|:;',<.>/?");
    const str4 = ifj.string("1234567890");
    const str5 = ifj.string("                 f           ");
    
    var str6 = ifj.string("d");
    
    str6 = ifj.concat(str1, str2);
    ifj.write(str6);
    ifj.write("\n");

    str6 = ifj.concat(str6, str3);
    ifj.write(str6);
    ifj.write("\n");

    str6 = ifj.concat(str6, str4);
    ifj.write(str6);
    ifj.write("\n");

    str6 = ifj.concat(str6, str5);
    ifj.write(str6);
    ifj.write("\n");

    var len = ifj.length(str1);
    ifj.write(len);
    ifj.write("\n");

    len = ifj.length(str2);
    ifj.write(len);
    ifj.write("\n");

    len = ifj.length(str3);
    ifj.write(len);
    ifj.write("\n");

    len = ifj.length(str4);
    ifj.write(len);
    ifj.write("\n");

    len = ifj.length(str5);
    ifj.write(len);
    ifj.write("\n");

    len = ifj.length(str6);
    ifj.write(len);
    ifj.write("\n");

    var i = 0;
    while(i < len){

        var j = i;
        while(j < len){
            var substr = ifj.substring(str6, i, j);
            var ord = ifj.ord(substr, 0);
            var ch = ifj.chr(ord);
            ifj.write(ch);
            ifj.write("\n");
            ifj.write(ord);
            ifj.write("\n");
            ifj.write(substr);
            ifj.write("\n");

            j = j + 1;
        }
        
        i = i + 1;
    }

    var g : ?f64 = ifj.readf64();
    if(g) |_G_26| {

        ifj.write(_G_26);
    }else{
        ifj.write("gecko zle nedasa cele zle\n");
    }

    ifj.write(\\testahjklfwdegrs
    \\aefshdgjvl
    \\sdlfgkkjnds
    );



}
