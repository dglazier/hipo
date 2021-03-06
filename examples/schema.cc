//***********************************************************************
//***********************************************************************
//    ____  ____  _____  _______     ___      ______       __
//   |_   ||   _||_   _||_   __ \  .'   `.   / ____ `.    /  |
//     | |__| |    | |    | |__) |/  .-.  \  `'  __) |    `| |
//     |  __  |    | |    |  ___/ | |   | |  _  |__ '.     | |
//    _| |  | |_  _| |_  _| |_    \  `-'  / | \____) | _  _| |_
//   |____||____||_____||_____|    `.___.'   \______.'(_)|_____|
//
//=======================================================================
// Autogenerated code by HIPO 3.1 io library
// Modify the main loop to suite your needs
// Date: 07/17/2018
// Author: G.Gavalian
//***********************************************************************
#include <cstdlib>
#include <iostream>
#include "dictionary.h"
#include "bank.h"

int main(int argc, char** argv) {

    hipo::schema  sch("rec::event",30,1);
    sch.parse(" pid/S,  px/F, py/F, pz/F, vx/F,vy/F,vz/F");
    sch.show();
    int item = 6;
    for(int i = 0 ;i < 6; i++){
      printf(" order = %4d, item = %4d, offset = %5d\n",
        i,item,sch.getOffset(item,i,6));
    }

    int size = sch.getSizeForRows(6);
    printf(" bank size = %d\n",size);

    hipo::bank dataBank(sch,12);
    dataBank.putInt("pid",0,11);
    dataBank.putInt("pid",1,211);
    dataBank.putShort("pid",2,-211);
    dataBank.putShort("pid",3,2212);
    dataBank.putInt("pid",4,22);
    dataBank.putInt("pid",5,22);

    dataBank.putFloat("px",8,7.1);
    dataBank.putFloat("py",8,8.2);
    dataBank.putFloat("pz",8,9.3);

    dataBank.putFloat("vx",0,1.1);
    dataBank.putFloat("vy",0,1.2);
    dataBank.putFloat("vz",0,1.3);

    dataBank.putFloat("vx",1,2.1);
    dataBank.putFloat("vy",1,2.2);
    dataBank.putFloat("vz",1,2.3);

    dataBank.show();

    std::string str = "pid/S,px/F,py/F,pz/F,vx/F,vy/F,vz/F";
    hipo::structure node(121,1,str);
    node.show();

    std::string dict = node.getStringAt(0);
    printf("result = '%s'\n",dict.c_str());

}
//### END OF GENERATED CODE
