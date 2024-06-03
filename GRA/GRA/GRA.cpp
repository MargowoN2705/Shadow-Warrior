#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <vector>
#include<Windows.h>

using namespace sf;
using namespace std;

class Bullet : public Sprite {
public:
    Vector2f velocity;

    Bullet(const sf::Texture& texture, const Vector2f& vel) {
        this->setTexture(texture);
        this->setTextureRect(sf::IntRect(0, 0, 50, 50));
        this->velocity = vel;
    }

    void move_bullet() {
        this->move(velocity);
    }
};

class Wall : public sf::Sprite {
public:
    Wall() = delete;
    Wall(float x, float y, const sf::Texture& texture) {
        this->setPosition(x, y);
        this->setTexture(texture);
        this->setTextureRect(sf::IntRect(0, 0, 65, 65)); 
    }

    void draw(RenderWindow& window) {
        window.draw(*this); 
    }
};

class Player : public Sprite {
public:
    int hp, dmg;
    float vel;
    Vector2f player_position;
    FloatRect bounds;
    std::vector<Bullet*> bullets;
    sf::Texture bulletTexture;

    Player() = default;
    virtual ~Player() {
        for (auto bullet : bullets) {
            delete bullet;
        }
    }


    void setMapBounds(const FloatRect& mapBounds) {
        bounds = mapBounds;
    }

    virtual void move_(const std::vector<Wall>& walls) {
        Vector2f pos = this->getPosition();
        Vector2f newPos = pos;

        if (Keyboard::isKeyPressed(Keyboard::D) && pos.x + vel < bounds.left + bounds.width) {
            newPos.x += vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::A) && pos.x - vel > bounds.left) {
            newPos.x -= vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::W) && pos.y - vel > bounds.top) {
            newPos.y -= vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::S) && pos.y + vel < bounds.top + bounds.height) {
            newPos.y += vel;
        }

        for (const auto& wall : walls) {
            if (wall.getGlobalBounds().intersects(FloatRect(newPos, this->getGlobalBounds().getSize()))) {

                return;
            }
        }


        this->setPosition(newPos);
    }



    void updateBullets(RenderWindow& window) {
        for (size_t i = 0; i < bullets.size(); ++i) {
            bullets[i]->move_bullet();
            window.draw(*bullets[i]);

            if (bullets[i]->getPosition().x < bounds.left || bullets[i]->getPosition().x > bounds.width ||
                bullets[i]->getPosition().y < bounds.top || bullets[i]->getPosition().y > bounds.height) {
                delete bullets[i];
                bullets.erase(bullets.begin() + i);
                --i;
            }
        }
    }

    virtual void SpecialAttack() = 0;
    virtual void attack() = 0;
};

class Wizzard : public Player {
private:
    Clock specialAttackTimer;
    bool specialAttackUsed = false;


public:
    Wizzard(int arg_hp, int arg_dmg, float arg_vel, const sf::Texture& texture, const sf::Texture& bulletTex) {
        this->setScale(0.20f, 0.20f);
        this->setTexture(texture);
        hp = arg_hp;
        dmg = arg_dmg;
        vel = arg_vel;
        bulletTexture = bulletTex;
    }

    void SpecialAttack() override {

        if (!specialAttackUsed) {
            srand(static_cast<unsigned>(time(NULL)));
            int posX = rand() % static_cast<int>(bounds.width);
            int posY = rand() % static_cast<int>(bounds.height);

            this->setPosition(bounds.left + posX, bounds.top + posY);

        }
    }

    void attack() override {
        Vector2f bulletDir;
        int load = 10;


        if (Keyboard::isKeyPressed(Keyboard::W)) {
            bulletDir = Vector2f(0, -5.0f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::S)) {
            bulletDir = Vector2f(0, 5.0f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::A)) {
            bulletDir = Vector2f(-5.0f, 0);
        }
        else if (Keyboard::isKeyPressed(Keyboard::D)) {
            bulletDir = Vector2f(5.0f, 0);
        }
        else {
            return;
        }

        if (bullets.size() < load)
        {
            Bullet* bullet = new Bullet(bulletTexture, bulletDir);
            bullet->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet->getGlobalBounds().width / 2, this->getPosition().y);
            bullet->setScale(2.0f, 2.0f);
            bool attackColision = false;
            for (auto& bullets : bullets)
            {
                if (bullets->getGlobalBounds().intersects(bullet->getGlobalBounds()))
                {
                    attackColision = true;
                    break;
                }

            }
            if (!attackColision)
            {
                bullets.push_back(bullet);
            }
        }


    }
};

class Elf : public Player {
public:
    Elf(int arg_hp, int arg_dmg, float arg_vel, const sf::Texture& texture, const sf::Texture& bulletTex) {
        this->setTexture(texture);
        this->setScale(2.25f, 2.25f);
        hp = arg_hp;
        dmg = arg_dmg;
        vel = arg_vel;
        bulletTexture = bulletTex;
    }



    void attack() override {
        Vector2f bulletDir;
        int load = 2;


        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            bulletDir = Vector2f(0, -15.0f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Down)) {
            bulletDir = Vector2f(0, 15.0f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left)) {
            bulletDir = Vector2f(-15.0f, 0);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            bulletDir = Vector2f(15.0f, 0);
        }
        else {
            return;
        }
        if (bullets.size() < load)
        {

            Bullet* bullet = new Bullet(bulletTexture, bulletDir);
            bullet->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet->getGlobalBounds().width / 2, this->getPosition().y);
            bullet->setScale(2.0f, 2.0f);
            bool attackColision = false;
            for (auto& bullets : bullets)
            {
                if (bullets->getGlobalBounds().intersects(bullet->getGlobalBounds()))
                {
                    attackColision = true;
                    break;
                }
            }
            if (!attackColision)
            {
                bullets.push_back(bullet);
            }
        }

    }

    void SpecialAttack() override {
        vel += 2.0;


        Bullet* bullet1 = new Bullet(bulletTexture, Vector2f(0.0f, 15.0f));
        Bullet* bullet2 = new Bullet(bulletTexture, Vector2f(0.0f, -15.0f));
        Bullet* bullet3 = new Bullet(bulletTexture, Vector2f(15.0f, 0.0f));
        Bullet* bullet4 = new Bullet(bulletTexture, Vector2f(-15.0f, 0.0f));

        bullet1->setScale(2.0f, 2.0f);
        bullet2->setScale(2.0f, 2.0f);
        bullet3->setScale(2.0f, 2.0f);
        bullet4->setScale(2.0f, 2.0f);

        bullet1->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet1->getGlobalBounds().width / 2, this->getPosition().y);
        bullet2->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet2->getGlobalBounds().width / 2, this->getPosition().y);
        bullet3->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet3->getGlobalBounds().width / 2, this->getPosition().y);
        bullet4->setPosition(this->getPosition().x + this->getGlobalBounds().width / 2 - bullet4->getGlobalBounds().width / 2, this->getPosition().y);

        if (bullets.size() < 4)
        {
            bullets.push_back(bullet1);
            bullets.push_back(bullet2);
            bullets.push_back(bullet3);
            bullets.push_back(bullet4);
        }
    }

    void move_(const std::vector<Wall>& walls) {
        Vector2f pos = this->getPosition();
        Vector2f newPos = pos;

        if (Keyboard::isKeyPressed(Keyboard::Right) && pos.x + vel < bounds.left + bounds.width) {
            newPos.x += vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::Left) && pos.x - vel > bounds.left) {
            newPos.x -= vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up) && pos.y - vel > bounds.top) {
            newPos.y -= vel;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down) && pos.y + vel < bounds.top + bounds.height) {
            newPos.y += vel;
        }

        for (const auto& wall : walls) {
            if (wall.getGlobalBounds().intersects(FloatRect(newPos, this->getGlobalBounds().getSize()))) {

                return;
            }
        }


        this->setPosition(newPos);
    }


};

class Enemy : public sf::Sprite {
private:
    Vector2f directionPrev;
    float velPrev;
public:
    int hp, dmg;
    float vel;
    Vector2f direction;
    FloatRect bounds;

    Enemy(int arg_hp, float arg_vel, int arg_dmg, const sf::Texture& texture) {
        this->setTexture(texture);
        hp = arg_hp;
        vel = arg_vel;
        dmg = arg_dmg;
        velPrev = arg_vel;

        srand(static_cast<unsigned>(time(NULL)));
        direction = Vector2f(static_cast<float>((rand() % 3) - 1), static_cast<float>((rand() % 3) - 1)); // -1, 0, 1
        if (direction.x == 0 && direction.y == 0) {
            direction = Vector2f(1.0f, 0.0f);
        }
    }

    void setMapBounds(const FloatRect& mapBounds) {
        bounds = mapBounds;
    }

    void move_enemy(float dt, const std::vector<Wall>& walls) {

        Vector2f newPos = this->getPosition() + direction * vel * dt + 0.5f * directionPrev * velPrev * dt * dt;



        if (newPos.x <= bounds.left || newPos.x + this->getGlobalBounds().width >= bounds.left + bounds.width) {
            direction.x = -direction.x; 
        }
        if (newPos.y <= bounds.top || newPos.y + this->getGlobalBounds().height >= bounds.top + bounds.height) {
            direction.y = -direction.y; 
        }
        for (const auto& wall : walls) {
            if (wall.getGlobalBounds().intersects(FloatRect(newPos, this->getGlobalBounds().getSize()))) {

                direction.x = -direction.x;
                direction.y = -direction.y;
            }
        }



        this->setPosition(newPos);


        directionPrev = direction;
    }
    bool CollisionPlayer(Player* player) {

        if (getGlobalBounds().intersects(player->getGlobalBounds())) {
            return true;
        }
        else
        {
            return false;
        }
    }


};

class Bonus : public sf::Sprite {
public: 
    Bonus() = delete; 
    Bonus(Texture& texture)
    {
        this->setTextureRect(IntRect(0.0f, 0.0f, 50.0f, 50.0f)); 
        this->setTexture(texture); 
        this->setPosition(rand()%1600, rand()%1000);
    }
    void drawBonus(RenderWindow& window)
    {
        window.draw(*this); 
    }
    void BonusSpecialEvent (Player& player)
    {
        player.vel += 2; 
    }

};

class Game {
private:
    RenderWindow window;
    Texture playerTexture1, playerTexture2, playerTextrue3, bulletTexture, mapTexture, enemyTexture, fireTexture, churchTexture, wallTexture,swordTexture;
    Sprite Map;
    FloatRect mapBounds;
    Player* player;
    Player* player2;
    vector<Enemy*> enemies;
    vector<Wall> walls;
    vector <Bonus> bonusVec; 
    vector <Sprite> backgroundMap;
    time_t start, end;
    Sprite sprite;
    int counter = 0;
    int bonusCounter = 0; 
    bool SpecialAttackUsed = false;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int screenWidth = desktop.width;
    unsigned int screenHeight = desktop.height;
    
    RectangleShape shape;

    sf::SoundBuffer enemyBuffer,welcometoHellBuffer, nowhereToRuNBuffer, onlyDemonsBuufer,searchBuffer,demonLaughBuffer, escapedBuffer,gameoverBuffer,enemydead1Buffer;
    sf::Sound enemySound,welcometoHellSound, nowhereToRunSound,onlyDemonsSound, searchSound, demonLaughSound, escapeSound,gameoverSound,enemydeadSound1;
    sf::SoundBuffer bonusBuffer;
    sf::Sound bonusSound;
    sf::SoundBuffer attackBuffer;
    sf::Sound attackSound;
    sf::SoundBuffer stepBuffer;
    sf::Sound stepSound;

    sf::Music backgroundMusic1, backgroundMusic2;

public:
    Game() : window(VideoMode(screenWidth, screenHeight), "Gra!", Style::Fullscreen) {
        window.setFramerateLimit(60);

        if (!playerTexture1.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Mage.png") ||
            !playerTexture2.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Warrior.png") ||
            !bulletTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\bullet.png") ||
            !mapTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\bushes.png") ||
            !enemyTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Knight.png") ||
            !playerTextrue3.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Elf2.png") ||
            !fireTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\flame.png") ||
            !churchTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\churchpreview1.png") ||
            !wallTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\bush2.png") ||
            !swordTexture.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\sword.png")
            )
        {
            exit(-1);
        }

        if (!attackBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\fireball.wav")||
            !bonusBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\coin.flac")||
            !welcometoHellBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL WELCOME TO HELL.wav") ||
            !stepBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\footsteps\\footsteps\\step_cloth2.ogg") ||
            !nowhereToRuNBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL NOWHERE TO RUN.wav") ||
            !onlyDemonsBuufer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL NO GODS ONLY DEMONS.wav") ||
            !searchBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL SEARCH EVERYWHERE.wav") ||
            !demonLaughBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL DEMON LAUGHS.wav") ||
            !escapedBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL YOU FOOLS  LET THEM ESCAPE.wav") ||
            !gameoverBuffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL GAME OVER.wav") ||
            !enemydead1Buffer.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\all_demon_lord_audio_files\\DL DEMON FIGHT GRUNT 1.wav")
            ) {
            std::cerr << "Error loading sound file" << std::endl;
        }

        if (!backgroundMusic1.openFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\fato_shadow_-_return_of_the_evil.ogg") ||
            !backgroundMusic2.openFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\fato_shadow_-_devil_inside.ogg"))
        {
            exit(-1); 
        }
        attackSound.setBuffer(attackBuffer);
        bonusSound.setBuffer(bonusBuffer); 
        welcometoHellSound.setBuffer(welcometoHellBuffer);
        stepSound.setBuffer(stepBuffer);
        nowhereToRunSound.setBuffer(nowhereToRuNBuffer);
        onlyDemonsSound.setBuffer(onlyDemonsBuufer);
        searchSound.setBuffer(searchBuffer);
        demonLaughSound.setBuffer(demonLaughBuffer); 
        escapeSound.setBuffer(escapedBuffer);
        gameoverSound.setBuffer(gameoverBuffer); 
        enemydeadSound1.setBuffer(enemydead1Buffer); 
        
        
        /*float currentVolumeStep = stepSound.getVolume();
        stepSound.setVolume(currentVolumeStep * 15.0f);*/

        float currentVolumeFire = attackSound.getVolume();
        attackSound.setVolume(currentVolumeFire *0.55f);
        float currentVolumeGameOver = gameoverSound.getVolume();
        gameoverSound.setVolume(currentVolumeGameOver * 3.25f);
        float currentVolumeEscape = escapeSound.getVolume();
        escapeSound.setVolume(currentVolumeEscape * 3.25f);

        Map.setTexture(mapTexture);
        Map.setScale(static_cast<float>(window.getSize().x) / Map.getLocalBounds().width,
            static_cast<float>(window.getSize().y) / Map.getLocalBounds().height);


        mapBounds = Map.getGlobalBounds();
        int los = rand() % 1;
        switch (los)
        {
        case 0: player = new Wizzard(15, 15, 5.1f, playerTexture1, fireTexture); player2 = new Elf(15, 15, 5.1f, playerTextrue3, fireTexture); break;
        case 1:  player = new Elf(15, 15, 5.1f, playerTextrue3, fireTexture); player2 = new Wizzard(15, 15, 5.1f, playerTexture1, fireTexture); break;
        }
        player->setPosition(rand() % 1600, rand() % 1000);
        player->setMapBounds(mapBounds);


        player2->setPosition(rand() % 1600, rand() % 1000);
        player2->setMapBounds(mapBounds);
        generateRandomWalls(20);
       
        generateRandomBonus(2500);
       
        for (auto& wall : walls)
        {
            if (player2->getGlobalBounds().intersects(wall.getGlobalBounds()))
            {
                wall.setPosition(0, 0);

            }

        }



        start = time(nullptr);



        shape.setPosition(rand() % 1500, rand() % 1000);

        shape.setTexture(&churchTexture);
        shape.setSize(Vector2f(175.0f, 175.0f));
      
        welcometoHellSound.play(); 

        
        int musicChoice = rand() % 2;  
        if (musicChoice == 0) {
            backgroundMusic1.setLoop(true);
            backgroundMusic1.play();
        }
        else {
            backgroundMusic2.setLoop(true);
            backgroundMusic2.play();
        }

    }

    ~Game() {
        delete player;
        delete player2;
        for (auto& enemy : enemies) {
            delete enemy;
        }
    }
    bool collisionWalls = false;

    void generateRandomWalls(int count) {
        srand(static_cast<unsigned>(time(nullptr)));
        for (int i = 0; i < count; ++i) {
            float x = static_cast<float>(rand() % static_cast<int>(mapBounds.width));
            float y = static_cast<float>(rand() % static_cast<int>(mapBounds.height));

            Wall newWall(x, y, wallTexture);
            bool collisionWalls = false;

            for (const auto& wall : walls) {
                if (newWall.getGlobalBounds().intersects(wall.getGlobalBounds()) || player->getGlobalBounds().intersects(wall.getGlobalBounds())) {
                    collisionWalls = true;
                    break;
                }
            }

            if (!collisionWalls) {
                walls.push_back(newWall);
            }
        }
    }


    void drawWalls() {
        for (auto& wall : walls) {

            window.draw(wall);
        }
    }
    bool collisionBonus = false; 
    void generateRandomBonus(int bonusInt)
    {
       for (int i =0; i < bonusInt; i++)
        {
           Bonus newBonus(swordTexture);
           newBonus.setScale(1.5f, 1.5f); 
         
           for (const auto& bonuses : bonusVec)
           {
               for(const auto& wall: walls)
               {
                   if (newBonus.getGlobalBounds().intersects(bonuses.getGlobalBounds()) || newBonus.getGlobalBounds().intersects(wall.getGlobalBounds()))
                   {
                       collisionBonus = true;
                   }
               }
           }
            if (!collisionBonus && bonusVec.size() < 4)
            {
                bonusVec.push_back(newBonus);
            }
        }

    }
    void drawBonus()
    {
        for (auto& bonus : bonusVec)
        {
            bonus.drawBonus(window); 
        }
    }

    void UpdateBonus()
    {
        for (auto it = bonusVec.begin(); it != bonusVec.end();)
        {
            if (player->getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                player->vel += 2;
                bonusCounter += 1; 
                it = bonusVec.erase(it); 
                bonusSound.play(); 
            }
            else if (player2->getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                player2->vel += 2;
                bonusCounter += 1; 
                it = bonusVec.erase(it); 
                bonusSound.play();
            }
            else
            {
                ++it; 
            }
        }
    }










    void CollisionB() {

        std::vector<Enemy*> enemiesToRemove;
        std::vector<Bullet*> bulletsToRemove;

        for (auto& enemy : enemies) {
            for (auto& bullet : player->bullets) {
                if (enemy->getGlobalBounds().intersects(bullet->getGlobalBounds()))
                {
                    enemydeadSound1.play();
                    enemiesToRemove.push_back(enemy);
                    bulletsToRemove.push_back(bullet);
                    counter++;
                   

                }
            }
        }

        for (auto& enemy : enemies) {
            for (auto& bullet : player2->bullets) {
                if (enemy->getGlobalBounds().intersects(bullet->getGlobalBounds()))
                {
                    enemydeadSound1.play();
                    enemiesToRemove.push_back(enemy);
                    bulletsToRemove.push_back(bullet);
                    counter++;

                }
            }
        }


        for (auto& enemy : enemiesToRemove) {
            enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());

        }

        for (auto& bullet : bulletsToRemove) {
            player->bullets.erase(std::remove(player->bullets.begin(), player->bullets.end(), bullet), player->bullets.end());

        }
    }




    void GameUpdateStats()
    {
        Font font;
        if (!font.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Arial.ttf")) {

        }
        time_t t = time(nullptr);
        double nowtime = difftime(t, this->start);
        Text time_text(to_string(static_cast<int>(nowtime)) + " s", font, 50);
        time_text.setFillColor(Color::Cyan);
        time_text.setStyle(Text::Bold);
        time_text.setPosition(0, 0);
        Text countert(to_string(this->counter), font, 50);
        countert.setFillColor(Color::White);
        countert.setStyle(Text::Bold);
        countert.setPosition(0, 50);
        Text counterE("enemies left: " + to_string(enemies.size()), font, 50);
        counterE.setFillColor(Color::White);
        counterE.setStyle(Text::Bold);
        counterE.setPosition(0, 100);
        window.draw(counterE);
        window.draw(time_text);
        window.draw(countert);

    }


    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }
            window.clear();
            window.draw(Map);
           
            
            spawnEnemy();
            window.draw(*player);
            window.draw(*player2);
            drawWalls();
            window.draw(shape);
            drawBonus(); 


            player->move_(walls);
            player2->move_(walls);

          /*  if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::D))
            {
                stepSound.play(); 
            }*/






            if (Wizzard* wizzardPtr = dynamic_cast<Wizzard*>(player))
            {
                if (Elf* elfPtr = dynamic_cast<Elf*>(player2))
                {
                    if (Keyboard::isKeyPressed(Keyboard::Space))
                    {
                        player->attack();
                        attackSound.play();
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Enter))
                    {
                        player2->attack();
                        attackSound.play();
                    }
                }
            }

            if (Elf* elfPtr = dynamic_cast<Elf*>(player))
            {
                if (Wizzard* wizzardPtr = dynamic_cast<Wizzard*>(player2))
                {
                    if (Keyboard::isKeyPressed(Keyboard::Space))
                    {
                        player2->attack();
                        attackSound.play(); 
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Enter))
                    {
                        player->attack();
                        attackSound.play();
                    }
                }
            }



            if (Keyboard::isKeyPressed(Keyboard::R)) {

                player->SpecialAttack();
                for (auto& wall : walls)
                {
                    if (player->getGlobalBounds().intersects(wall.getGlobalBounds()))
                    {
                        player->setPosition(rand() % 1600, rand() % 1000);
                    }
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::P))
            {

                if (!SpecialAttackUsed)
                {
                    player2->SpecialAttack();
                    attackSound.play(); 
                    SpecialAttackUsed = true;
                }
            }
            player->updateBullets(window);
            player2->updateBullets(window);


            updateEnemies();
            UpdateBonus(); 
            CollisionB();
            GameUpdateStats();


            if (rand() % 175 == 0)
            {
                int los = rand() % 4;
                    switch (los)
                    {
                    case 0 : nowhereToRunSound.play(); break;
                    case 1:  onlyDemonsSound.play(); break; 
                    case 2:  searchSound.play(); break; 
                    case 3:  demonLaughSound.play(); break; 
                    default: break; 


                    }
            }






            if (shape.getGlobalBounds().intersects(player->getGlobalBounds()) && bonusCounter >=3)
            {
                GameWin = true;
            }




            if (GameOver || GameWin)
            {
                Font font;
                if (!font.loadFromFile("C:\\Users\\barte\\OneDrive\\Pulpit\\Gra\\Arial.ttf")) {

                }
                end = time(nullptr);
                double durr = difftime(end, this->start);
                RenderWindow gameOverWindow(VideoMode(screenWidth, screenHeight), "GAME OVER!", Style::Fullscreen);

                Text Time_("You Survived " + to_string(static_cast<int>(durr)) + " s", font, 75);
                Time_.setFillColor(Color::Green);
                Time_.setStyle(Text::Bold);
                Time_.setPosition((screenWidth - Time_.getLocalBounds().width) / 2,
                    ((screenHeight - Time_.getLocalBounds().height) / 2) + 100);
                Text KilledMobs("You killed " + to_string(this->counter) + " mobs", font, 75);
                KilledMobs.setFillColor(Color::Green);
                KilledMobs.setStyle(Text::Bold);
                KilledMobs.setPosition((screenWidth - KilledMobs.getLocalBounds().width) / 2,
                    ((screenHeight - KilledMobs.getLocalBounds().height) / 2) + 200);

                if (GameOver)
                {
                    
                    Sprite gamoverTexture(enemyTexture);
                    gamoverTexture.setPosition(0, 100); 
                    gamoverTexture.setScale(50.0f, 50.0f); 
                    gameOverWindow.draw(gamoverTexture); 
                    Text gameOverText("GAME OVER", font, 100);
                    gameOverText.setFillColor(Color::Red);
                    gameOverText.setStyle(Text::Bold);
                    gameOverText.setPosition((screenWidth - gameOverText.getLocalBounds().width) / 2,
                        (screenHeight - gameOverText.getLocalBounds().height) / 2);
                    gameOverWindow.draw(gameOverText);
                    gameOverWindow.draw(Time_);
                    gameOverWindow.draw(KilledMobs);
                    gameOverWindow.display();
                    Sleep(500); 
                    gameoverSound.play();
                }
                if (GameWin)
                {
                    Sprite gamewinTexture1(playerTextrue3);
                    gamewinTexture1.setPosition(0, 100);
                    gamewinTexture1.setScale(25.0f, 25.0f);
                    gameOverWindow.draw(gamewinTexture1);
                    Sprite gamewinTexture2(playerTexture1);
                    gamewinTexture2.setPosition(1200, 200);
                    gamewinTexture2.setScale(2.0f, 2.0f);
                    gameOverWindow.draw(gamewinTexture2);
                    Text WinText("Congratulations : You Won", font, 100);
                    WinText.setFillColor(Color::Red);
                    WinText.setStyle(Text::Bold);
                    WinText.setPosition((screenWidth - WinText.getLocalBounds().width) / 2,
                        (screenHeight - WinText.getLocalBounds().height) / 2);
                    gameOverWindow.draw(WinText);
                    gameOverWindow.draw(Time_);
                    gameOverWindow.draw(KilledMobs);
                    gameOverWindow.display();
                    Sleep(500); 
                    escapeSound.play();
                }
               



               

               
                window.close();
                Sleep(10000);

            }
            else
            {
                window.display();
            }


        }
    }

private:
    bool GameOver = false;
    bool GameWin = false;
    void spawnEnemy() {
        if (rand() % 10 == 0) {
            Enemy* enemy = new Enemy(10, 1.0f, 10, enemyTexture);
            enemy->setPosition(rand() % 1500, rand() % 1000);
            enemy->setMapBounds(mapBounds);
            enemy->setScale(6.5f, 6.5f);

            bool collisionEnemy = false;


            for (const auto& thisEnemy : enemies) {
                if (enemy->getGlobalBounds().intersects(thisEnemy->getGlobalBounds())) {
                    collisionEnemy = true;
                    break;
                }
            }


            if (!collisionEnemy) {
                for (const auto& thisWall : walls) {
                    if (enemy->getGlobalBounds().intersects(thisWall.getGlobalBounds())) {
                        collisionEnemy = true;
                        break;
                    }
                }
            }

            if (!collisionEnemy) {
                enemies.push_back(enemy);
            }
            else {
                delete enemy;
            }
        }
    }


    void updateEnemies() {

        static bool Protected = false;
        static time_t startProtected;

        for (auto e = enemies.begin(); e != enemies.end();) {
            (*e)->move_enemy(1.75f, walls);


            if (player2->getGlobalBounds().intersects(shape.getGlobalBounds())) {
                if (!Protected) {
                    Protected = true;
                    startProtected = time(nullptr);
                }
            }


            if (Protected) {
                time_t endProtected = time(nullptr);
                if (difftime(endProtected, startProtected) > 5) {
                    Protected = false;
                }
            }

            if ((((*e)->CollisionPlayer(player2)) || ((*e)->CollisionPlayer(player))) && !Protected) {
                //GameOver = true;
            }

            if (counter == 420 && bonusCounter == 3)
            {
                GameWin = true;
            }

            if ((*e)->hp <= 0) {
                delete* e;
                e = enemies.erase(e);
            }
            else {
                window.draw(**e);
                ++e;
            }
        }
    }
};

int main() {
    srand(time(nullptr));
    Game game;
    game.run();

    return 0;
}
