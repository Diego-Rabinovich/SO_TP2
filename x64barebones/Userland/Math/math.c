#include "include/math.h"
#include "../User/include/userLib.h"

#define SUM "sum"
#define SUB "sub"
#define DIV "div"
#define MUL "mul"
#define POT "pot"
#define MOD "mod"
#define ANS "ans"

int ans = 0;

signed int math(char* operator, char* op1, char* op2, signed int* res){
    signed int num1 = 0, num2 = 0;

    //Chequeo que sean numeros o se quiera usar el ans
    if(!strToInt(op1, &num1)){
        if(strCmp(op1, ANS) != 0){
            sys_write(2, "FIRST OPERAND IS INVALID", 24, 0, 2);
            return 0;
        } else{
            num1 = ans;
        }
    }
    if(!strToInt(op2, &num2)){
        if(strCmp(op2, ANS) != 0){
            sys_write(2, "SECOND OPERAND IS INVALID", 25, 0, 2);
            return 0;
        } else{
            num2 = ans;
        }
    }

    if(strCmp(operator, SUM) == 0){
        *res = num1 + num2;
    }
    else if(strCmp(operator, SUB) == 0){
        *res = num1 - num2;
    }
    else if(strCmp(operator, MUL) == 0){
        *res = num1*num2;
    }
    else if(strCmp(operator, DIV) == 0){
            *res = num1/num2;
    }
    else if(strCmp(operator, POT) == 0){
        signed int aux = 1;
        for (int i = 0; i < num2; ++i) {
            aux *= num1;
        }
        *res = aux;
    }
    else if(strCmp(operator, MOD) == 0){
        *res = num1%num2;
    }
    else{
        //INVALID OP EXCEPTION
        sys_write(2, "INVALID OPERATION", 17, 0, 2);
        return 0;
    }

    ans = *res;
    return 1;
}
