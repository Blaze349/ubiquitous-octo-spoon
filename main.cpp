#include <iostream>
#include <fstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
class chip8 {
    private:
        unsigned char memory[4096];
        unsigned int pc;
        unsigned char V[16]; // data registers
        unsigned short I; //address register
        unsigned short stack[24];
        unsigned short sp;
        unsigned int timer;
        unsigned int soundTimer;
        unsigned char keyboard[16];
        unsigned char gfx[4096];
        sf::Clock clock;
        double fps = 1e6f/60.f;
        bool drawFlag = false;
        unsigned char fontSet[80] = {
          0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
          0x20, 0x60, 0x20, 0x20, 0x70, // 1
          0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
          0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
          0x90, 0x90, 0xF0, 0x10, 0x10, // 4
          0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
          0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
          0xF0, 0x10, 0x20, 0x40, 0x40, // 7
          0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
          0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
          0xF0, 0x90, 0xF0, 0x90, 0x90, // A
          0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
          0xF0, 0x80, 0x80, 0x80, 0xF0, // C
          0xE0, 0x90, 0x90, 0x90, 0xE0, // D
          0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
          0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    public:
        chip8() {
            pc = 0x200;
            I = 0;
            sp = 0;
            for (int i = 0; i < (int)sizeof fontSet / (int)sizeof fontSet[0]; i++) {
                memory[i] = fontSet[i];
            }
            for (int i = 0; i < 2048; i++) gfx[i] = 0;
        }
        void loop(sf::RenderWindow& window) {
            input(window);
            while(window.isOpen()) {
                input(window);
                opcode(window);
                if (clock.getElapsedTime().asMicroseconds() > fps) {
                    clock.restart();
                    soundTimer--;
                    timer--;
                }
                if (drawFlag) {
                    std::cout << "Drawing" << std::endl;
                    drawScreen(window);
                }
            }
        }
        void drawScreen(sf::RenderWindow& window) {
            drawFlag = false;
            window.clear(sf::Color::Black);
            float xtileoffset = 800.0/64.0;
            float ytileoffset = 600.0/32.0;
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    sf::RectangleShape tile(sf::Vector2f(xtileoffset, ytileoffset));
                    tile.setPosition(x * xtileoffset, y * ytileoffset);

                    for (int i = 0; i < 4; i++) {
                        if (gfx[x + (y * 64)] == 0) tile.setFillColor(sf::Color::Black);
                        else tile.setFillColor(sf::Color::White);                    
                    }


                    window.draw(tile);
                }
            }
            window.display();
        }
        sf::Keyboard::Key translateToSfml(char x) {
            switch(x) {
                case 0x0:
                    return sf::Keyboard::Num1;
                break;
                case 0x1:
                    return sf::Keyboard::Num2;
                break;
                case 0x3: 
                    return sf::Keyboard::Num3;
                break;
                case 0x4:
                    return sf::Keyboard::Num4;
                break;
                case 0x5:
                    return sf::Keyboard::Q;
                break;
                case 0x6:
                    return sf::Keyboard::W;
                break;
                case 0x7:
                    return sf::Keyboard::E;
                break;
                case 0x8:
                    return sf::Keyboard::R;
                break;
                case 0x9:
                    return sf::Keyboard::A;
                break;
                case 0xA:
                    return sf::Keyboard::S;
                break;
                case 0xB:
                    return sf::Keyboard::D;
                break;
                case 0xC:
                    return sf::Keyboard::F;
                break;
                case 0xD:
                    return sf::Keyboard::Z;
                break;
                case 0xE:
                    return sf::Keyboard::X;
                break;
                case 0xF:
                    return sf::Keyboard::C;
                break;
                default:
                    std::cerr << "Keyboard condition not checked" << std::endl;
                break;
            }
        }
        unsigned char translateToKeys(sf::Keyboard::Key key) {
            switch(key) {
                case sf::Keyboard::Num1:
                    return 0x0; 
                break;
                case sf::Keyboard::Num2:
                    return 0x1; 
                break;
                case sf::Keyboard::Num3: 
                    return 0x3;
                break;
                case sf::Keyboard::Num4:
                    return 0x4; 
                break;
                case sf::Keyboard::Q:
                    return 0x5;
                break;
                case sf::Keyboard::W:
                    return 0x6;
                break;
                case sf::Keyboard::E:
                    return 0x7;
                break;
                case sf::Keyboard::R:
                    return 0x8;
                break;
                case sf::Keyboard::A:
                    return 0x9;
                break;
                case sf::Keyboard::S:
                    return 0xA;
                break;
                case sf::Keyboard::D:
                    return 0xB;
                break;
                case sf::Keyboard::F:
                    return 0xC;
                break;
                case sf::Keyboard::Z:
                    return 0xD;
                break;
                case sf::Keyboard::X:
                    return 0xE;
                break;
                case sf::Keyboard::C:
                    return 0xF;
                break;
                default:
                    std::cerr << "Keyboard condition not checked" << std::endl;
                    return -1;
            }
        }

        void keyboardInput(unsigned char x) {
            keyboard[x] = sf::Keyboard::isKeyPressed(translateToSfml(x));
        } 
        void loadFile(char* dir) {
            std::cout << "Loading file\n";
            std::ifstream rom(dir, std::ios::in | std::ios::binary);
            if (rom) {
                rom.seekg(0, std::ios::end);
                int size = rom.tellg();
                if (size > 0xFFF - 0x200) {
                    std::cerr << "Ram is too large" << std::endl;
                    exit(1);
                }
                else {
                    char* romm = (char* )(&(memory[0x200]));
                    rom.seekg(0, std::ios::beg); //back to beginning
                    rom.read(romm, size);
                }
            }
            else {
                std::cerr << "No file found" << std::endl;
                exit(1);
            }
            rom.close(); 
        }

        void clearScreen(sf::RenderWindow& window) {
            window.clear(sf::Color::Black);
            for (int i = 0; i < 2048; i++) gfx[i] = 0;
        }

        void draw(char x, char y, char height) {
            unsigned short pixel;
            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                std::cout << I + yline << "\n";
                for (int xX = 0; xX < 8; xX++) {
                    if ((pixel & (0x80 >> xX)) != 0) {
                        int gfxCoord = x + xX + ((y + yline) * 64);
                        if (gfx[gfxCoord] == 1) V[0xF] = 1;
                        printf("%d\n", gfx[gfxCoord]);
                        gfx[gfxCoord] ^= 1;
                    } 
                }
            }

            drawFlag = true;
        }

        void input(sf::RenderWindow& window) {
            sf::Event event;
            while(window.pollEvent(event)) {
                std::cout << "Event here";
                if (event.type == sf::Event::Closed) window.close();
                else if (event.type == sf::Event::KeyPressed) {
                    unsigned char index = translateToKeys(event.key.code);
                    if (index >= 0x0 && index <= 0xF) {
                        keyboard[index] = 1;
                    } 
                }
                else if (event.type == sf::Event::KeyReleased) {
                    unsigned char index = translateToKeys(event.key.code);
                    if (index >= 0x0 && index <= 0xF) {
                        keyboard[index] = 0;
                    } 
                }
            }
        }
        void opcode(sf::RenderWindow& window) {
            unsigned short opcode = memory[pc] << 8 | memory[pc+1];
            printf("Doing opcode %X \n", opcode);
            unsigned int x = (opcode & 0x0F00) >> 4;
            unsigned int y = (opcode & 0x00F0) >> 8;
            unsigned int NNN = opcode & 0x0FFF;
            unsigned int NN = opcode & 0x00FF;

            switch (opcode & 0xF000) {
                case 0x0000:
                    switch(opcode & 0x00FF) {
                        case 0x00E0:
                            for (int i = 0; i < 2048; i++) gfx[i] = 0x0;
                            drawFlag = true;
                            pc += 2;
                        break;
                        case 0x00EE:
                            --sp;
                            pc = stack[sp];
                            pc += 2;
                        break;
                        default:
                            printf("Unknown opcode, %X at program counter number %X \n", opcode, pc);
                            exit(2);
                    } 
                break;
                case 0x1000:
                    pc = opcode & 0x0FFF;
                break;
                case 0x2000:
                    stack[sp] = pc;
                    ++sp;
                    pc = opcode & 0x0FFF;
                break;
                case 0x3000:
                    if (V[x] == NN) pc += 4;
                    else pc += 2;
                break;
                case 0x4000:
                    if (V[x] != NN) pc += 4;
                    else pc += 2;
                break;
                case 0x5000:
                    if (V[x] == V[y]) pc += 4; 
                    else pc += 2;
                break;
                case 0x6000:
                    V[x] = NN;
                    pc += 2;
                break;

                case 0x7000:
                    V[x] += NN;
                    pc += 2;
                break;

                case 0x8000: // bitwise ops
                    switch(opcode & 0x000F) {
                        case 0x0000:
                            V[x] = V[y];
                            pc += 2;
                        break;

                        case 0x0001:
                            V[x] = V[x] | V[y];
                            pc += 2;
                        break;

                        case 0x0002:
                            V[x] = V[x] & V[y];
                            pc += 2;
                        break;
                        
                        case 0x0003:
                            V[x] = V[x] ^ V[y];
                            pc += 2;
                        break;   
                        
                        case 0x0004:
                            if (V[y] > (0xFF - V[x])) V[0xF] = 1;
                            else V[0xF] = 0;
                            V[x] += V[y];
                            pc += 2;
                        break;
                        
                        case 0x0005:
                            if (V[y] > V[x]) V[0xF] = 0;
                            else V[0xF] = 1;
                            V[x] -= V[y];
                            pc += 2;
                        break;    

                        case 0x0006:
                            V[0xF] = V[x] & 1;
                            V[x] = V[x] >> 1;
                            pc += 2;
                        break;
                        case 0x0007:
                            if (V[x] > V[y]) V[0xF] = 0;
                            else V[0xF] = 1;
                            V[y] -= V[x];
                            pc += 2;
                        break; 
                        
                        case 0x000E:
                            V[0xF] = V[x] & 0xF000;
                            V[x] = V[x] << 1;
                            pc += 2;
                        break;
                        default:
                            printf("Unknown opcode, %X at program counter number %X \n", opcode, pc);
                            exit(2);
                    }
                break;

                case 0x9000:
                    if (V[x] != V[y]) pc += 4; 
                    else pc += 2;
                break; 

                case 0xA000:
                    I = NNN;
                    pc += 2;
                break;

                case 0xB000:
                    pc = V[0] + NNN; 
                break;

                case 0xC000:
                    V[x] = (rand() % 256) & NN;
                    pc += 2;
                break;

                case 0xD000:
                    draw(V[x], V[y], opcode & 0x000F);
                    pc += 2;
                break;
                
                case 0xE000:
                    switch(opcode & 0x00FF) {
                        case 0x009E:
                            if (keyboard[V[x]] == 1) pc += 4;
                            else pc += 2;
                        break;
                        case 0x00A1:
                            if (keyboard[V[x]] == 0) pc += 4;
                            else pc += 2;

                        break;

                        default:
                            printf("Unknown opcode, %X at program counter number %X \n", opcode, pc);
                            exit(2);
                    }
                break;

                case 0xF000:
                    switch(opcode & 0x00FF) {
                        case 0x0007:
                            V[x] = timer;
                            pc += 2;
                        break;

                        case 0x000A:
                        {
                                bool keyPressed = false;
                                for (int i = 0; i < 16; i++) {
                                    if (keyboard[i] == 1) {
                                        V[x] = i;
                                        keyPressed = true;
                                    }
                                } 
                                if (keyPressed) {
                                    pc += 2;
                                } else {
                                    return;
                                }

                        }
                       break;

                        case 0x0015:
                            timer = V[x];
                            pc += 2; 
                        break;

                        case 0x0018:
                            soundTimer = V[x];
                            pc += 2;
                        break;

                        case 0x001E:
                            I += V[x];
                            pc += 2;
                        break;

                        case 0x0029:
                            //complete later
                            I = 0x050 + V[x] * 5;
                            pc += 2;
                        break;
                        
                        case 0x0033:
                            memory[I] = V[x] / 100;
                            memory[I + 1] = (V[x] / 10) % 10;
                            memory[I + 2] = (V[x] % 100) % 10;
                            pc += 2;
                        break;

                        case 0x0055:
                            for (int i = 0; i < 16; i++) {
                                memory[I + i] = V[i];
                            }
                            pc += 2;
                        break;

                        case 0x0065:
                            for (int i = 0; i < 16; i++) {
                                V[i] = memory[I + i];
                            }
                            pc += 2;
                        break;
                    }
                break;
                default:
                    printf("Unknown opcode, %X at program counter number %X \n", opcode, pc);
                    exit(2);

            }
        }
};

int main(int argc, char** argv) {
    chip8 Chip8;
    if (argc > 1) Chip8.loadFile(argv[1]);
    else Chip8.loadFile("./games/PONG"); 

    sf::RenderWindow window(sf::VideoMode(800, 600), "Chip-8 Window");
    Chip8.loop(window);
    return 0;
}



