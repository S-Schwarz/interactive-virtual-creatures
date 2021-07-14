//
// Created by st on 6/19/21.
//

#include <iostream>

#include "App/App.h"

int main(){

    ivc::App mainApp = ivc::App();
    if(mainApp.init() < 0){
        std::cout << "Failed to start App!" << std::endl;
        return -1;
    }

    while(!mainApp.shouldClose()){
        mainApp.update();
    }

    mainApp.close();

    return 0;
}