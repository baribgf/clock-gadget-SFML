#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <thread>
#include <time.h>

using namespace std;
using namespace sf;

typedef struct
{
    int width;
    int height;
} size;

size get_image_size(string file_path)
{
    size s;
    unsigned char buf[8];
    ifstream in(file_path);
    in.seekg(16);
    in.read(reinterpret_cast<char *>(&buf), 8);
    s.width = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
    s.height = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);
    return s;
}

void update_hands(RectangleShape *s, RectangleShape *m, RectangleShape *h)
{
    while (true)
    {
        time_t now = time(nullptr);
        int secs = localtime(&now)->tm_sec;
        int mins = localtime(&now)->tm_min;
        int hours = localtime(&now)->tm_hour;
        s->setRotation(360.0 / (60.0 / secs) - 180);
        m->setRotation(360.0 / (60.0 / mins) - 180);
        if (hours > 12)
            h->setRotation(360.0 / (12.0 / (hours - 12)) - 180);
        else if (hours < 12)
            h->setRotation(360.0 / (12.0 / (hours)) - 180);
        else if (hours == 0)
            h->setRotation(360.0 - 180);
    }
}

int main(int argc, char const *argv[])
{
    RenderWindow window(VideoMode(200, 200), "MyClock", Style::None, ContextSettings(24, 8, 4));
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    Event event;

    Sprite clock;
    Texture t;
    string t_path = "./resources/clock.png";
    size t_size = get_image_size(t_path);
    t.loadFromFile(t_path, IntRect(0, 0, t_size.width, t_size.height));
    t.setSmooth(true);
    clock.setTexture(t);
    clock.scale(Vector2f((float)window.getSize().x / t.getSize().x, (float)window.getSize().y / t.getSize().y));

    RectangleShape s_line;
    s_line.setSize(Vector2f(2, window.getSize().y / 2.0 * 0.75));
    s_line.setPosition(Vector2f((window.getSize().x) / 2.0 + s_line.getSize().x, (window.getSize().y) / 2.0));
    s_line.setFillColor(Color::Red);
    RectangleShape m_line;
    m_line.setSize(Vector2f(2, window.getSize().y / 2.0 * 0.7));
    m_line.setPosition(Vector2f((window.getSize().x) / 2.0 + m_line.getSize().x, (window.getSize().y) / 2.0));
    m_line.setFillColor(Color::Blue);
    RectangleShape h_line;
    h_line.setSize(Vector2f(3, window.getSize().y / 2.0 * 0.5));
    h_line.setPosition(Vector2f((window.getSize().x) / 2.0 + h_line.getSize().x, (window.getSize().y) / 2.0));
    h_line.setFillColor(Color::Black);

    thread t1(update_hands, &s_line, &m_line, &h_line);
    t1.detach();

    Font font;
    font.loadFromFile("./resources/UbuntuMono-R.ttf");

    Text text;
    string str = "BariClock";
    text.setString(str);
    text.setFont(font);
    text.setFillColor(Color::Black);
    text.setCharacterSize(16);
    text.setPosition(Vector2f((window.getSize().x - text.getLocalBounds().width) / 2, window.getSize().y * 0.65));

    bool LeftMouseIsPressed = false;
    Vector2i RelativeMouseCoords;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
            if (event.type == Event::Closed)
                window.close();

            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();

            if (event.type == Event::MouseButtonPressed && Mouse::isButtonPressed(Mouse::Left))
            {
                LeftMouseIsPressed = true;
                RelativeMouseCoords = Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
            }

            if (event.type == Event::MouseButtonReleased && !Mouse::isButtonPressed(Mouse::Left))
            {
                LeftMouseIsPressed = false;
            }

            if (LeftMouseIsPressed)
            {
                Vector2i NewPosition = Vector2i(Mouse::getPosition().x - RelativeMouseCoords.x, Mouse::getPosition().y - RelativeMouseCoords.y);
                window.setPosition(NewPosition);
            }

        window.clear();

        window.draw(clock);
        window.draw(text);
        window.draw(h_line);
        window.draw(m_line);
        window.draw(s_line);

        window.display();
    }
    return 0;
}
