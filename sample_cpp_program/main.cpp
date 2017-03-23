#include <iostream>
#include <limits>

#define INITIAL_STATE 1
#define INTERMEDIATE_STATE 2

int main() {
    int state = INITIAL_STATE;
    int choice = 0;
    while (state != 3) {
        choice = 0;
        std::cout << "Current state is: " << state << std::endl;
        for (;;) {
            std::cout << "Choose state (INITIAL_STATE = 1,INTERMEDIATE_STATE = 2,STATE_EXIT = 3): " << std::endl;
            if (std::cin >> choice && choice > 0 && choice <= 3) {
                break;
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        std::cout << "Action chosen: " << choice << std::endl;
        if (choice == 1) {
            state = INITIAL_STATE;
        } else if (choice == 2) {
            state = INTERMEDIATE_STATE;
        } else if (choice == 3) {
            state = INITIAL_STATE;
        }
        std::cout << "Transitioned to state: " << state << std::endl;
    }
    return 0;
}