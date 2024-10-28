
#include "symtable.h"
#include <stdio.h>

int main(){
    Elem_id **GlobSymTable;
    GlobSymTable = TableInit();
    
    Elem_id *Output;
    Output = TableSearch("test", 1, GlobSymTable);
    printf("%p\n", Output);

    Elem_id Elem;

    Elem.name = "B";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 1;
    Elem.FnVar.Fn_id.LocalSymTable = TableInit();
    TableAdd(Elem, GlobSymTable);


    Elem.name = "G";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 2;
    Elem.FnVar.Fn_id.LocalSymTable = TableInit();
    TableAdd(Elem, GlobSymTable);

    Elem.name = "L";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 3;
    Elem.FnVar.Fn_id.LocalSymTable = TableInit();
    TableAdd(Elem, GlobSymTable);

    Output = TableSearch("L", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    Elem.name = "Q";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 4;
    Elem.FnVar.Fn_id.LocalSymTable = TableInit();
    TableAdd(Elem, GlobSymTable);

    Elem.name = "L";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 10;
    Elem.FnVar.Fn_id.LocalSymTable = NULL;
    TableAdd(Elem, GlobSymTable);

    Elem.name = "V";
    Elem.level = 0;
    Elem.Type = FUNCTION;
    Elem.FnVar.Fn_id.data = 5;
    Elem.FnVar.Fn_id.LocalSymTable = TableInit();
    TableAdd(Elem, GlobSymTable);

    Output = TableSearch("B", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    Output = TableSearch("G", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    Output = TableSearch("L", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    Output = TableSearch("Q", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    Output = TableSearch("V", 0, GlobSymTable);
    printf("%p\n", Output);
    printf("%s\n", Output->name);
    printf("%d\n", Output->level);
    printf("%d\n", Output->Type);
    printf("%d\n", Output->FnVar.Fn_id.data);
    printf("%p\n", Output->FnVar.Fn_id.LocalSymTable);

    TableClear(GlobSymTable, FUNCTION);

    return 0;
}
