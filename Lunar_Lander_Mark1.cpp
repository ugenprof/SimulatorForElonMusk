#include "Lunar_Lander_Mark1.h"

Lunar_Lander_Mark1::Lunar_Lander_Mark1(sf::Vector2f position) : Ship("Lunar_Lander_Mark1.png", download(position)) {
    assembly();
}

RigidBodyParameters Lunar_Lander_Mark1::download(sf::Vector2f position) {
    std::ifstream fin("data/Lunar_Lander_Mark1.txt");
    if (!fin.is_open()) { throw "Wrong file name"; }
    RigidBodyParameters par;
    par.position = position;
    std::string str;
    float x, y;
    fin >> str >> par.width;
    fin >> str >> par.height;
    fin >> str >> par.angle;
    fin >> str >> par.mass;
    fin >> str >> par.moment_of_inertia;
    fin >> str >> x >> y;
    par.mass_position = Vector2f(x, y);
    fin.close();
    return par;
}

void Lunar_Lander_Mark1::assembly() {
    AddEngine(Engine(Object("test3.png", Vector2f(10, 10), 20, 60, 0), Vector2f(0.5, 1),
        Force(false, 400, Vector2f(0, -1), Vector2f(0.5, 1)), 10), "1");
    AddEngine(Engine(Object("test3.png", Vector2f(0, 0), 30, 10, 180), Vector2f(0, 0),
        Force(false, 200, Vector2f(-1, 0), Vector2f(0, 0)), 10), "3");
    AddEngine(Engine(Object("test3.png", Vector2f(0, 0), 30, 10, 180), Vector2f(1, 0),
        Force(false, 200, Vector2f(1, 0), Vector2f(1, 0)), 10), "4");
    AddEngine(Engine(Object("test3.png", Vector2f(0, 0), 30, 10, 180), Vector2f(1, 1),
        Force(false, 200, Vector2f(1, 0), Vector2f(1, 1)), 10), "5");
    AddEngine(Engine(Object("test3.png", Vector2f(0, 0), 30, 10, 180), Vector2f(0, 1),
        Force(false, 200, Vector2f(-1, 0), Vector2f(0, 1)), 10), "6");
    AddForce("0", Force(true, 100, Vector2f(0, 400), Vector2f(0, 0)));
    ForceOn("0");
}

void Lunar_Lander_Mark1::control() {
    if (Keyboard::isKeyPressed(Keyboard::W)) {
        ForceOn("1");
    }
    else {
        EngineOff("1");
    }
    if (Keyboard::isKeyPressed(Keyboard::Q)) {
        EngineOn("3");
        EngineOn("5");
    }
    else {
        EngineOff("3");
        EngineOff("5");
    }
    if (Keyboard::isKeyPressed(Keyboard::E)) {
        EngineOn("4");
        EngineOn("6");
    }
    else {
        EngineOff("4");
        EngineOff("6");
    }
    if (Keyboard::isKeyPressed(Keyboard::Num1)) {
        SetEngineThrust("1", 1);
    }
    if (Keyboard::isKeyPressed(Keyboard::Num2)) {
        SetEngineThrust("1", 0.5);
    }
    if (Keyboard::isKeyPressed(Keyboard::Num3)) {
        SetEngineThrust("1", 0.25);
    }
    if (Keyboard::isKeyPressed(Keyboard::D) && Keyboard::isKeyPressed(Keyboard::A)) {
        SetEngineThrustAngle("1", 0);
    }
    else if (Keyboard::isKeyPressed(Keyboard::A)) {
        SetEngineThrustAngle("1", 1);
    }
    else if (Keyboard::isKeyPressed(Keyboard::D)) {
        SetEngineThrustAngle("1", -1);
    }
    else {
        SetEngineThrustAngle("1", 0);
    }
}

void Lunar_Lander_Mark1::drow_all(RenderWindow& window, bool position, bool speed, bool way, bool forces, bool collision) {
    if (position == true) {
        DrawMassPosition(window);
    }
    if (way == true) {
        DrawBodyWay(window);
    }
    if (speed == true) {
        DrawSpeed(window);
    }
    if (forces == true) {
        DrawForce(window, GetForce("1"));
        DrawForce(window, GetForce("3"));
        DrawForce(window, GetForce("4"));
        DrawForce(window, GetForce("5"));
        DrawForce(window, GetForce("6"));
        DrawForce(window, GetForce("0"));
    }
    if (collision == true) {
        CollisionModelDrow(window);
    }
}
