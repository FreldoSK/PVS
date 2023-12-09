#include "main.h"

int main() {
    std::shared_ptr<Uart> uart = std::make_shared<Uart>(USBTX, USBRX);
    std::shared_ptr<Details> details = std::make_shared<Details>();


    boardOrJoy(uart, details);
    gamePlay(uart, details);
}


void boardOrJoy(std::shared_ptr<Uart>& uart, const std::shared_ptr<Details>& details) {
    uart->writeMessage("Do you wanna use keyboard ? y/n");
    uart->readMessage();
    std::unique_ptr<Joystick> joystick = std::make_unique<Joystick>();
    
    while(true) {
        if (uart->getChar() == 'y' || joystick->getResponse() == 2) {
            uart->clearScreen();
            initStartBoard(uart, details);    
        } else {
            uart->clearScreen();
            initStartJoy(uart, details, joystick);
        }
    }
}


void gamePlay(std::shared_ptr<Uart>& uart, const std::shared_ptr<Details>& details) {

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    uint8_t actualCapacityOfTable = 0;
    uint8_t * tableBuffer = (uint8_t*) malloc (sizeof(uint8_t) * details->capacityOfTable);
    uint8_t * typeCounter = (uint8_t*) malloc (sizeof(uint8_t) * NUM_OF_CLASSES);
    uint8_t * epicWeapons = (uint8_t*) malloc (sizeof(uint8_t) * NUM_OF_CLASSES);


    for(int i=0; i < 4; i++) {
        typeCounter[i] = 0;
        epicWeapons[i] = 0;
    }

    Thread shop_thread;
    Thread heroes_thread[details->numberOfHeroes];

    std::vector<std::shared_ptr<Hero>> heroes;

    std::shared_ptr<Data> data = std::make_unique<Data>();
    
    std::shared_ptr<Table> table = std::make_shared<Table>(tableBuffer, actualCapacityOfTable, details->capacityOfTable, data);
    std::shared_ptr<Shop> shop = std::make_shared<Shop>(details->shopTime, details->numberOfHeroes, uart);
  
    for (int i = 0; i < details->numberOfHeroes; i++) {
        TypOfHeroe randomHeroType = static_cast<TypOfHeroe>(std::rand() % (ARCHER + 1));
        uint8_t idOfHero = 1 + rand() % details->numberOfHeroes;
        uint8_t chance = 1 + rand() % 10;

        heroes.push_back(std::make_shared<Hero>(idOfHero, randomHeroType, uart, details->heroTime, chance, typeCounter, epicWeapons));
    }

    shop_thread.start([&]() {
        shop->shopFunction(table);
    });


    for (int i = 0; i < details->numberOfHeroes; i++) {
        heroes_thread[i].start([&, i]() {
        heroes[i]->heroFunction(table);
    });
}


    Button * button = new Button();
    Led * led = new Led(LED1, LED2);
    Joystick * joystick = new Joystick();

    while(true) {
        if (button->getSituation()) {
            uart->printResult(typeCounter, epicWeapons);


            if (uart->getChar() == 'y' || joystick->getResponse() == 2) {   
                free(tableBuffer);
                free(typeCounter);
                free(epicWeapons);
                delete button;
                delete joystick;
                uart->clearScreen();
                boardOrJoy(uart, details);
            } else {
                for(int i=0; i < 3; i++) {
                    led->redLight(true);
                    ThisThread::sleep_for(1s);
                }

                led->redLight(false);    
                uart->writeMessage("Thanks for playing !!!");
                exit(0);
                
            }
        }
        led->ledPWM(100);
    }
    
    led->greenLight(false);
    led->redLight(false);

    delete button;
    delete led;
    delete joystick;
    free(tableBuffer);
    free(typeCounter);
    free(epicWeapons);
    exit(0);
}


void initStartBoard(std::shared_ptr<Uart>& uart, const std::shared_ptr<Details>& details) {
    uart->writeMessage("==========|-POTION QUESTS SIMMULATION-|==========");
    uart->writeMessage("[1] Select number of heroes !");
    uart->readMessage();
    details->numberOfHeroes = uart->getCounter();
    uart->setCounter(-1);

    uart->writeMessage("[2] Select capacity of table !");
    uart->readMessage ();

    details->capacityOfTable = uart->getCounter();
    if (details->capacityOfTable == 0) {
        uart->writeMessage("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
        exit(0);
    }

    uart->setCounter(-1);

    uart->writeMessage("[3] Select time for hero delay !");
    uart->readMessage();
    details->heroTime = uart->getCounter();
    uart->setCounter(-1);

    uart->writeMessage("[4] Select delay for shop!");
    uart->readMessage();
    details->shopTime = uart->getCounter();
    uart->setCounter(-1);
    uart->clearScreen(); 
    gamePlay(uart, details);                     
}

void initStartJoy(std::shared_ptr<Uart>& uart, const std::shared_ptr<Details>& details, const std::unique_ptr<Joystick>& joystick) {
    uart->writeMessage("==========|-POTION QUESTS SIMMULATION-|==========");
    uart->writeMessage("[1] Select number of heroes !");
    joystick->joyPressed(uart, details);
    details->numberOfHeroes = joystick->getCounter();
    joystick->setCounter(0);

    uart->writeMessage("[2] Select capacity of table !");
    joystick->joyPressed(uart, details);
    details->capacityOfTable = joystick->getCounter();
    joystick->setCounter(0);

    uart->writeMessage("[3] Select time for hero delay !");
    joystick->joyPressed(uart, details);
    details->heroTime = joystick->getCounter();
    joystick->setCounter(0);

    uart->writeMessage("[4] Select delay for shop!");
    joystick->joyPressed(uart, details);
    details->shopTime = joystick->getCounter();
    joystick->setCounter(0);
    uart->clearScreen(); 
    gamePlay(uart, details); 
}