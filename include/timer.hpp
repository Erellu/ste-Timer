
/**
                                ste::Timer
    
    @short This class provides a thread-based C++ timer that calls a user-defined function.

    @details
    Features:
        • Can call any function.
        • Single shot or loop execution editable at runtime.
        • Function change while running (provided the two decltype()s are identical).

    @copyright     Copyright (C) <2020-2021>  <DUHAMEL Erwan>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

     @author DUHAMEL Erwan (erwanduhamel@outlook.com)
*/



/**

EXAMPLE :


#include "timer.hpp"

#include <functional>

#include <iostream>


int main(){


    std::function<void(void)> a_fun = []{std::cout << "A" << std::endl;};
    std::function<void(void)> b_fun = []{std::cout << "B" << std::endl;};

    ste::Timer t(a_fun , std::chrono::milliseconds(1000) , false); //Declare the timer to call 'a_fun' each 1000 milliseconds indefinitely.

    t.start(); //Start the timer

    std::this_thread::sleep_for(std::chrono::milliseconds(2100)); //Wait for 2100ms

    t.function() = b_fun; //Change the function called by 't'

    std::this_thread::sleep_for(std::chrono::milliseconds(2100)); //Wait for 2100ms

    return 0;
}


//OUTPUT :
//
//A
//A
//B
//B




//    NOTE : If you use 'auto' to deduce type from lambdas, you may encounter compilation errors due to lambda types being a bit special.
//    It is recommended to use std::function<ReturnType<Arg,...>> as a template parameter IF YOU INTEND TO CHANGE THE FUNCTION AT RUNTIME. Otherwise it does not matter.

*/




#ifndef STE_TIMER_HPP
#define STE_TIMER_HPP

#include <chrono>

#include <thread>

#include <type_traits>

namespace ste {


template<class Function> //Best way to pass function as argument without knowing anything about it
class Timer {

    static_assert (std::is_invocable<Function>::value, "ste::Timer can only be initialized with an invocable template parameter." );

private:

    bool _stopped;                       //Current state of the timer.
    bool _is_single_shot;                //Timer mode (continuous calls to the function or single call after the delay) 
    std::chrono::milliseconds _ms_delay; //Interval between calls in milliseconds
    Function _function;                  //Function to call

public:

    /**********************************************************/
    ///                      Constructor
    /**********************************************************/


    /**
        @brief Default constructor.
        @details If last argument is not specified, the timer will execute the function until its lifetime expires. Otherwise, it will only be executed one time after the specified delay.
    */
    Timer(Function function,
          const std::chrono::milliseconds &_delay,
          const bool &single_shot = true
         )
        :
          _stopped(true),             //ste::Timer is stopped when declared.
          _is_single_shot(single_shot),
          _ms_delay(_delay),
          _function(function)
    {};



    /**********************************************************/
    ///                      Functions
    /**********************************************************/

    /**
         @brief Starts the timer and returns a reference to it.
         @details Nothing happens if the timer is already started.
         @return Reference to the timer.
    */
    Timer& start(){ //Starts the timer

        if(_stopped){         //Is the timer stopped ?
            _stopped = false; //The timer is now running
            on_run();       //Initialize thread
        }
        return (*this);
    }

    /**
        @brief Stops the timer and returns a reference to it.
        @return Reference to the timer.
    */
    Timer& stop(){
        _stopped = true;
        return (*this);
    }

    /**********************************************************/
    ///                   Accessors / Mutators
    /**********************************************************/

    /**
        @brief Returns the current state of the timer.
        @return 'true' if the timer is running, 'false' otherwise
    */
    bool running() const {return !stopped();}


    /**
        @brief Returns the current state of the timer.
        @return 'true' if the timer is stopped, 'false' otherwise
    */
    bool stopped() const{return _stopped;}



    /**
        @brief Accessor for the attribute holding the execution policy of the timer (single shot or continuous).
        @return 'const' reference to the attribute. 'true' if the timer is single shot, 'false' otherwise.
    */
    const bool& is_single_shot() const{return _is_single_shot;}


    /**
        @brief Accessor for the attribute holding the execution policy of the timer (single shot or continuous).
        @return Reference to the attribute. 'true' if the timer is single shot, 'false' otherwise.
    */
    bool& is_single_shot(){return _is_single_shot;}



    /**
        @brief Accessor for the attribute holding the current function called by the timer.
        @return Reference to the the current function called by the timer.
    */
    Function& function(){return _function;}

    /**
        @brief Accessor for the attribute holding the current function called by the timer.
        @return 'const' reference to the the current function called by the timer.
    */
    const Function& function() const{return _function;}


protected:

    /**
        @brief Launchs a thread that calls periodically the function called by the timer.
        @details This function is virtual. You may override it in subclasses.
    */
    virtual void on_run(){

        if(_is_single_shot){

            std::thread t(
            [=]() { //Lambda to describe action while running
                if(_stopped){return;} //Return if stopped here
                std::this_thread::sleep_for(_ms_delay); //Wait
                if(_stopped){return;} //Return if stopped here
                _function();
                _stopped = true; //Stop
                return; //Exit thread
            }
            );
            t.detach(); //Permits the thread to execute independently from the thread handle
        }

        /*+++++++++++*/

        else{

            std::thread t(
            [=]() { //Lambda to describe action while running
                while(!_is_single_shot) {//Also return if set to single shot mode while in the loop
                    if(_stopped){return;}
                    std::this_thread::sleep_for(_ms_delay);
                    if(_stopped){return;}
                    _function();
                }
                _stopped = true;
                return;
            }
            );
            t.detach(); //Permits the thread to execute independently from the thread handle
        }

    }

};


}
#endif //STE_TIMER_HPP