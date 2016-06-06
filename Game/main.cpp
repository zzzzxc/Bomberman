#include<iostream>
#include<string>
#include<conio.h>
#include<sstream>
#include<fstream>
#include<vector>
#include<map>
#include<algorithm>
#include <SFML/Graphics.hpp>
#include<forward_list>
#include<Thor/Animations/FrameAnimation.hpp>
#include<Thor/Animations/Animator.hpp>
#define ROWS 15
#define COLS 21
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define BOMB_LIMIT 10
#define MAX_BOMB_RADIUS 5
#define MAX_SPEED 4
#define BOMB
#define TICKS_IN_SECS 3
#define DROP_PROBABILITY 2
#define ENEMY_SPEED 1
#define KICK_SPEED 4

//Converts World Coordinates to Tile Coordinates
sf::Vector2i worldToTileCoord(sf::Vector2f position) {
	int x = position.x / TILE_WIDTH;
	int y = position.y / TILE_HEIGHT;
	return sf::Vector2i(x, y);
}

class Tile{
public:
	enum TileType{ Grass, Box, Solid, HiddenPortal, RevealedPortal, Drop };
	Tile() {
		holdingBomb = false;
		explosionTime = sf::seconds(0);
		fireDuration = sf::seconds(1);
	}
	Tile(float x, float y, int width, int height, sf::Texture* texture) {
		this->width = width;
		this->height = height;
		i = y / height;
		j = x / width;
		holdingBomb = holdingDrop = false;
		explosionTime = sf::seconds(0);
		fireDuration = sf::seconds(0.3f);
		this->texture = texture;
		texture->setSmooth(true);
		secondarySprite.setTexture(*texture);
		secondarySprite.setPosition(TILE_WIDTH*j, TILE_HEIGHT*i);
		secondarySprite.setTextureRect(sf::IntRect(0, 0, TILE_WIDTH, TILE_HEIGHT));
		secondarySprite.setScale(0.25f, 0.25f);
	}
	bool containsWorldCoord(sf::Vector2f position) {
		return (position.y / height == i && position.x / width == j);
	}
	bool isWalkable() {
		return (tileType == Grass || tileType == RevealedPortal || tileType == Drop);
	}
	bool isOnFire(sf::Time currentTime) {
		if (explosionTime.asSeconds() == 0) {
			return false;
		}
		if (isDestructible() && currentTime.asSeconds() - explosionTime.asSeconds() <= fireDuration.asSeconds()) {
			return true;
		}
		if (currentTime.asSeconds() - explosionTime.asSeconds() <= fireDuration.asSeconds()) {
			holdingBomb = false;
			return true;
		}
		return false;
	}
	bool isHoldingBomb() {
		return holdingBomb;
	}
	bool isDestructible() {
		return (tileType == Box || tileType == Drop || tileType == HiddenPortal);
	}
	bool isHoldingDrop() {
		return holdingDrop;
	}
	void setExplosionTime(sf::Time explosionTime) {
		this->explosionTime = explosionTime;
	}
	void setTileType(int tileType) {
		this->tileType = tileType;
	}
	void receiveBomb() {
		holdingBomb = true;
	}
	void removeBomb() {
		holdingBomb = false;
	}
	void receiveDrop(int dropType) {
		this->dropType = dropType;
		if (dropType == -1)
			holdingDrop = false;
		else {
			holdingDrop = true;
			tileType = Drop;
		}
	}
	int getTileType() {
		return tileType;
	}
	int getDropType() {
		return dropType;
	}
	sf::Sprite& getSprite() {
		return sprite;
	}
	sf::Sprite& getSecondarySprite() {
		return secondarySprite;
	}
	void explode(sf::Vector2f bombPos, int bombRadius, sf::Time explosionTime) {
		holdingBomb = false;
		holdingDrop = false;
		new (&explosion) thor::FrameAnimation();
		new (&animator) thor::Animator<sf::Sprite, std::string>();
		this->explosionTime = explosionTime;
		sf::Vector2i bombTile = worldToTileCoord(bombPos);
		int distX, distY;
		distX = abs(j - bombTile.x);
		distY = abs(i - bombTile.y);
		int frameWidth = 130, frameHeight = 130;
		//for (int i=0; i<=23; i++) {
		//	//	explosion.addFrame(1.f, sf::IntRect(0, i*frameHeight, frameWidth, frameHeight));
		//}
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				explosion.addFrame(1.f, sf::IntRect(j*frameWidth, i*frameHeight, frameWidth, frameHeight));
			}
		}
		if (tileType == TileType::HiddenPortal) {
			tileType = TileType::RevealedPortal;
			sprite.setTextureRect(sf::IntRect(0, 2 * TILE_HEIGHT - 1, TILE_WIDTH, TILE_HEIGHT));
		}

		//if (distX == 0 && distY == 0) {
		//	for (int i=0; i<=23; i++) {
		//		
		//		explosion.addFrame(1.f, sf::IntRect(0, i*frameHeight, frameWidth, frameHeight));
		//	}
		//}
		//else if (distX == bombRadius && distY == 0) {
		//	if (j < bombTile.x) {
		//		//for (int i=6; i>=2; i--) {
		//		explosion.addFrame(1.f, sf::IntRect(4*frameWidth+4, 6*frameHeight + 7, frameWidth, frameHeight));
		//		//}
		//	}
		//	else if (j > bombTile.x) {
		//		//for (int i=1; i<=5; i++) {
		//			explosion.addFrame(1.f, sf::IntRect(2*frameWidth + 2, 9*frameHeight + 9, frameWidth, frameHeight));
		//		//}
		//	}
		//}
		//else if (distX == 0 && distY == bombRadius) {
		//	if (i < bombTile.y) {
		//		//for (int i=1; i<=5; i++) {
		//			explosion.addFrame(1.f, sf::IntRect(0*frameWidth+1, 9*frameHeight + 8, frameWidth, frameHeight));
		//		//}
		//	}
		//	else if (i > bombTile.y) {
		//		//for (int i=6; i>=2; i--) {
		//			explosion.addFrame(1.f, sf::IntRect(6*frameWidth+6+1, 6*frameHeight + 7, frameWidth, frameHeight));
		//		//}
		//	}
		//}
		//else if (distX != 0) {
		//	//for (int i=1; i>=5; i++) {
		//		explosion.addFrame(1.f, sf::IntRect(1*frameWidth, 2*frameHeight + 3, frameWidth, frameHeight));
		//	//}
		//}
		//else if (distY != 0) {
		//	//for (int i=6; i>=2; i--) {
		//		explosion.addFrame(1.f, sf::IntRect(1*frameWidth, 2*frameHeight + 3, frameWidth, frameHeight));
		//	//}
		//}
		animator.addAnimation("explosion", explosion, fireDuration);
	}
	thor::Animator<sf::Sprite, std::string>& getAnimator() {
		return animator;
	}
	sf::Vector2f getWorldPosition() {
		return sf::Vector2f(j*width, i*height);
	}
	sf::Vector2i getIndex() {
		return sf::Vector2i(j, i);
	}
private:
	int i, j;
	int width, height, tileType, dropType;
	bool holdingBomb, holdingDrop;
	sf::Texture* texture, invertedTexture;
	sf::Sprite sprite, secondarySprite;
	sf::Time explosionTime, fireDuration;
	thor::FrameAnimation explosion;
	thor::Animator<sf::Sprite, std::string> animator;
};

class Bomberman{
public:
	Bomberman(sf::Vector2i spawnTile, bool AI, std::string filename, sf::Clock& clock) {
		texture.loadFromFile(filename);
		texture.setSmooth(true);
		sprite.setTexture(texture);
		this->clock = clock;
		this->AI = AI;
		lives = 3;
		speed = 2.0f;
		dead = semiGhost = kick = false;
		int frameWidth = 16, frameHeight = 32;
		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
		for (int i = 0; i < texture.getSize().y / frameHeight; i++) {
			for (int j = 0; j < texture.getSize().x / frameWidth; j++) {
				sf::IntRect frameRect(frameWidth*j, frameHeight*i, frameWidth, frameHeight);
				if (i == 0) {
					down.addFrame(1.f, frameRect);
				}
				else if (i == 1) {
					up.addFrame(1.f, frameRect);
				}
				else if (i == 2) {
					right.addFrame(1.f, frameRect);
				}
				else if (i == 3) {
					left.addFrame(1.f, frameRect);
				}
			}
		}
		kick = false;
		collision = false;
		idle.addFrame(1.f, sf::IntRect(0, 0, frameWidth, frameHeight));
		animator.addAnimation("down", down, sf::seconds(1.f));
		animator.addAnimation("up", up, sf::seconds(1.f));
		animator.addAnimation("left", left, sf::seconds(1.f));
		animator.addAnimation("right", right, sf::seconds(1.f));
		animator.addAnimation("idle", idle, sf::seconds(1.f));
		prevDirection = direction = IDLE;
		sprite.setScale(1.5, 1.5);
		sprite.setPosition(spawnTile.x*TILE_WIDTH, spawnTile.y*TILE_HEIGHT - 16);
		prevPos = sprite.getPosition();
		respawnTile = spawnTile;
	}
	void incrementLives() {
		lives++;
	}
	void death() {
		timeOfDeath = clock.getElapsedTime();
		lives--;
		dead = true;
	}
	void setLatestBombTile(sf::Vector2i latestBombTile) {
		this->latestBombTile = latestBombTile;
	}
	sf::Vector2i getLatestBombTile() {
		return latestBombTile;
	}

	void setNewPathTime(sf::Time newPathTime) {
		this->newPathTime = newPathTime;
	}
	sf::Time getNewPathTime() {
		return newPathTime;
	}
	bool isDead() {
		return dead;
	}


	bool hasCollided() {
		return collision;
	}

	void setCollision(bool collision) {
		this->collision = collision;
	}

	void respawn() {
		if (dead && clock.getElapsedTime().asSeconds() - timeOfDeath.asSeconds() >= 4) {
			sprite.setPosition(respawnTile.x*TILE_WIDTH, respawnTile.y*TILE_HEIGHT - 16);
			prevDirection = direction = IDLE;
			animator.stopAnimation();
			kick = false;
			dead = false;
			speed = 2.0f;
		}
	}
	enum Direction{ UP, DOWN, LEFT, RIGHT, IDLE };
	void setDirection(int direction) {
		prevDirection = this->direction;
		this->direction = direction;
	}
	int getDirection() {
		return direction;
	}
	void draw(sf::RenderWindow& window, sf::Time dt) {
		if (dead) return;
		if (prevDirection != direction) {
			switch (direction) {
			case UP:
				if (!animator.isPlayingAnimation() || animator.getPlayingAnimation() != "up")
					animator.playAnimation("up", true);
				break;
			case DOWN:
				if (!animator.isPlayingAnimation() || animator.getPlayingAnimation() != "down")
					animator.playAnimation("down", true);
				break;
			case LEFT:
				if (!animator.isPlayingAnimation() || animator.getPlayingAnimation() != "left")
					animator.playAnimation("left", true);
				break;
			case RIGHT:
				if (!animator.isPlayingAnimation() || animator.getPlayingAnimation() != "right")
					animator.playAnimation("right", true);
				break;
			case IDLE:
				animator.playAnimation("idle");
				break;
			}
		}
		animator.update(dt);
		animator.animate(sprite);
		window.draw(sprite);
	}
	void setSemiGhostMode(bool semiGhost) {
		this->semiGhost = semiGhost;
	}
	bool isIntelligent() {
		return AI;
	}

	bool isSemiGhost() {
		return semiGhost;
	}
	void setKick(bool kick) {
		this->kick = kick;
	}
	bool canKick() {
		return kick;
	}

	int getLives() {
		return lives;
	}
	void setPosition(float x, float y) {
		prevPos = sprite.getPosition();
		sprite.setPosition(x, y);
	}
	int getSpeed() {
		return speed;
	}
	void setSpeed(float speed) {
		this->speed = speed;
	}
	sf::Vector2i getTile() {
		return worldToTileCoord(sf::Vector2f((getPosition().x + 5) + ((getSprite().getTextureRect().width - 10) / 2), (getPosition().y + 25) + (getSprite().getTextureRect().height - 20)));
	}

	void setPathForAI(std::forward_list<Tile*> path) {
		this->path = path;
	}
	void move() {
		if (AI) {
			if (!path.empty()) {
				sf::Vector2i currentTile = getTile();
				for (std::forward_list<Tile*>::iterator it = path.begin(); it != path.end(); it++) {
					if ((*it)->getIndex() == currentTile) it++;
					else continue;
					if (it == path.end()) break;
					//if (static_cast<int>(sprite.getPosition().x) % 32 != 0 || static_cast<int>(sprite.getPosition().y) % 32 != 0) {
					//	continue;
					//}
					if ((*it)->getIndex().x == currentTile.x && (*it)->getIndex().y == currentTile.y - 1 && (*it)->isWalkable()) {
						//move up
						direction = UP;
					}
					else if ((*it)->getIndex().x == currentTile.x && (*it)->getIndex().y == currentTile.y + 1 && (*it)->isWalkable()) {
						//move down
						std::cout << getTile().x << " " << getTile().y << std::endl;
						direction = DOWN;
					}
					else if ((*it)->getIndex().y == currentTile.y && (*it)->getIndex().x == currentTile.x - 1 && (*it)->isWalkable()) {
						//move left
						direction = LEFT;
					}
					else if ((*it)->getIndex().y == currentTile.y && (*it)->getIndex().x == currentTile.x + 1 && (*it)->isWalkable()) {
						//move right
						direction = RIGHT;
					}
				}
			}
		}
	}
	sf::Vector2f getPosition() {
		return sprite.getPosition();
	}
	sf::Vector2f getPrevPosition() {
		return prevPos;
	}
	sf::Sprite& getSprite() {
		return sprite;
	}
	thor::Animator<sf::Sprite, std::string>& getAnimator() {
		return animator;
	}
protected:
	int lives;
	int prevDirection, direction;
	float speed;
	bool dead, semiGhost, kick, collision, AI;
	sf::Vector2i respawnTile, latestBombTile;
	std::forward_list<Tile*> path;
	sf::Vector2f prevPos;
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Clock clock;
	sf::Time timeOfDeath, newPathTime;
	thor::FrameAnimation up, down, left, right, idle;
	thor::Animator<sf::Sprite, std::string> animator;
};





class Bomb{
public:
	Bomb(sf::Vector2f position, sf::Texture& texture, sf::Time plantTime, Tile** map) {
		sprite.setTexture(texture);
		int frameWidth = 16, frameHeight = 16;
		sprite.setTextureRect(sf::IntRect(0 * frameWidth, 8 * frameHeight + 7, frameWidth, frameHeight));
		sprite.setPosition(position);
		ticking.addFrame(1.f, sf::IntRect(0 * frameWidth, 8 * frameHeight + 7, frameWidth, frameHeight));
		ticking.addFrame(1.f, sf::IntRect(1 * frameWidth, 8 * frameHeight + 7, frameWidth, frameHeight));
		ticking.addFrame(1.f, sf::IntRect(2 * frameWidth + 1, 8 * frameHeight + 7, frameWidth + 1, frameHeight));
		ticking.addFrame(1.f, sf::IntRect(1 * frameWidth, 8 * frameHeight + 7, frameWidth, frameHeight));
		ticking.addFrame(1.f, sf::IntRect(0 * frameWidth, 8 * frameHeight + 7, frameWidth, frameHeight));
		this->plantTime = plantTime;
		this->map = map;
		animator.addAnimation("ticking", ticking, sf::seconds(1.5));
		sprite.setScale(1.5, 1.5);
		kicked = false;
		prevPos = position;
	}
	sf::Vector2f getPrevPosition() {
		return prevPos;
	}
	sf::Vector2f getPosition() {
		return sprite.getPosition();
	}
	sf::Sprite& getSprite() {
		return sprite;
	}
	sf::Time getPlantTime() {
		return plantTime;
	}

	int getBombRadius() {

	}
	enum Direction{ UP, DOWN, LEFT, RIGHT };
	void move() {
		prevPos = sprite.getPosition();
		switch (kickDirection) {
		case UP:
			sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - KICK_SPEED);
			break;
		case DOWN:
			sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + KICK_SPEED);
			break;
		case LEFT:
			sprite.setPosition(sprite.getPosition().x - KICK_SPEED, sprite.getPosition().y);
			break;
		case RIGHT:
			sprite.setPosition(sprite.getPosition().x + KICK_SPEED, sprite.getPosition().y);
			break;
		}
	}
	int getKickDirection() {
		return kickDirection;
	}
	bool isKicked() {
		return kicked;
	}
	void kick(int direction) {
		kicked = true;
		kickDirection = direction;
	}
	void stop() {
		kicked = false;
	}
	void explode(int bombRadius, sf::Time explosionTime) {
		sf::Vector2i currTile = worldToTileCoord(sprite.getPosition());
		bool up, down, left, right;
		up = down = left = right = true;
		map[currTile.y][currTile.x].explode(sprite.getPosition(), bombRadius, explosionTime);
		for (int i = 1; i <= bombRadius; ++i) {
			if (left && currTile.x - i >= 0) {
				//left
				if (map[currTile.y][currTile.x - i].isDestructible()) {
					map[currTile.y][currTile.x - i].explode(sprite.getPosition(), bombRadius, explosionTime);
					left = false;
				}
				else if (!map[currTile.y][currTile.x - i].isWalkable()) {
					left = false;
				}
				else {
					map[currTile.y][currTile.x - i].explode(sprite.getPosition(), bombRadius, explosionTime);
				}
			}
			if (right && currTile.x + i < COLS) {
				//right
				if (map[currTile.y][currTile.x + i].isDestructible()) {
					map[currTile.y][currTile.x + i].explode(sprite.getPosition(), bombRadius, explosionTime);
					right = false;
				}
				else if (!map[currTile.y][currTile.x + i].isWalkable()) {
					right = false;
				}
				else {
					map[currTile.y][currTile.x + i].explode(sprite.getPosition(), bombRadius, explosionTime);
				}
			}
			if (down && currTile.y + i < ROWS) {
				//down
				if (map[currTile.y + i][currTile.x].isDestructible()) {
					map[currTile.y + i][currTile.x].explode(sprite.getPosition(), bombRadius, explosionTime);
					down = false;
				}
				else if (!map[currTile.y + i][currTile.x].isWalkable()) {
					down = false;
				}
				else {
					map[currTile.y + i][currTile.x].explode(sprite.getPosition(), bombRadius, explosionTime);
				}
			}
			if (up && currTile.y - i >= 0) {
				//up
				if (map[currTile.y - i][currTile.x].isDestructible()) {
					map[currTile.y - i][currTile.x].explode(sprite.getPosition(), bombRadius, explosionTime);
					up = false;
				}
				else if (!map[currTile.y - i][currTile.x].isWalkable()) {
					up = false;
				}
				else {
					map[currTile.y - i][currTile.x].explode(sprite.getPosition(), bombRadius, explosionTime);
				}
			}
		}
	}



	thor::Animator<sf::Sprite, std::string>& getAnimator() {
		return animator;
	}
private:
	int bombRadius;
	int kickDirection;
	bool kicked;
	Tile** map;
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Time plantTime;
	sf::Vector2f prevPos;
	thor::FrameAnimation ticking;
	thor::Animator<sf::Sprite, std::string> animator;
};

class BombManager{
public:
	BombManager(Tile** map, sf::Texture& bombs, sf::Clock& clock) {
		this->map = map;
		this->clock = clock;
		this->texture = bombs;
		texture.setSmooth(true);
		bombRadius = 1;
		bombLimit = 1;
		bombCount = 0;
	}
	void addBomb(sf::Vector2f position) {
		if (bombCount < bombLimit) {
			Bomb* bomb = new Bomb(position, texture, clock.getElapsedTime(), map);
			bombsPlanted.push_front(bomb);
			sf::Vector2i bombTile = worldToTileCoord(position);
			map[bombTile.y][bombTile.x].receiveBomb();
			bombCount++;
		}
	}

	void deleteBomb(sf::Vector2f bombPosition) {
		if (!bombsPlanted.empty()) {
			std::forward_list<Bomb*>::iterator it = bombsPlanted.begin();
			while (it != bombsPlanted.end()) {
				sf::Time timeTicked = clock.getElapsedTime() - (*it)->getPlantTime();
				sf::Vector2i bombTile = worldToTileCoord((*it)->getPosition());
				if (worldToTileCoord(bombPosition) == bombTile) {
					map[bombTile.y][bombTile.x].removeBomb();
					delete (*it);
					bombsPlanted.remove(*(it++));
					bombCount--;
					break;
				}
				else {
					++it;
				}
			}
		}
	}
	int getBombCount() {
		return bombCount;
	}
	std::forward_list<Bomb*>& getBombs() {
		return bombsPlanted;
	}
	int getBombRadius() {
		return bombRadius;
	}
	void resetBombLimit() {
		bombLimit = 1;
	}
	void resetBombRadius() {
		bombRadius = 1;
	}
	int getBombLimit() {
		return bombLimit;
	}
	void setBombRadius(int bombRadius) {
		this->bombRadius = bombRadius;
	}
	void setBombLimit(int bombLimit) {
		this->bombLimit = bombLimit;
	}
	void draw(sf::RenderWindow& window, sf::Time dt) {
		if (!bombsPlanted.empty()) {
			std::forward_list<Bomb*>::iterator it = bombsPlanted.begin();
			while (it != bombsPlanted.end()) {
				sf::Time timeTicked = clock.getElapsedTime() - (*it)->getPlantTime();
				sf::Vector2i bombTile = worldToTileCoord((*it)->getPosition());
				if (timeTicked.asSeconds() >= TICKS_IN_SECS || map[bombTile.y][bombTile.x].isOnFire(clock.getElapsedTime())) {
					(*it)->explode(bombRadius, clock.getElapsedTime());
					delete (*it);
					bombsPlanted.remove(*(it++));
					bombCount--;
				}
				else {
					++it;
				}
			}
		}
		for (std::forward_list<Bomb*>::iterator it = bombsPlanted.begin();
			it != bombsPlanted.end(); it++) {
			(*it)->getAnimator().animate((*it)->getSprite());
			(*it)->getAnimator().update(dt);
			if (!(*it)->getAnimator().isPlayingAnimation()) {
				(*it)->getAnimator().playAnimation("ticking", true);
			}
			window.draw((*it)->getSprite());
		}
	}
	bool checkLocationForExplosions(sf::Vector2i locationTile) {
		bool safe = true;
		bool up, down, left, right;
		for (std::forward_list<Bomb*>::iterator it = bombsPlanted.begin();
			it != bombsPlanted.end(); it++) {
			sf::Vector2i bombTile = worldToTileCoord((*it)->getPosition());
			up = down = left = right = true;
			if (locationTile == bombTile) {
				std::cout << "not safe!\n";
				return true;
			}
			for (int i = 1; i <= bombRadius; ++i) {
				if (left && bombTile.x - i >= 0) {
					//left
					if (map[bombTile.y][bombTile.x - i].isDestructible()) {
						left = false;
					}
					else if (!map[bombTile.y][bombTile.x - i].isWalkable()) {
						left = false;
					}
					else if (locationTile.x == bombTile.x - i && locationTile.y == bombTile.y && clock.getElapsedTime().asSeconds() - (*it)->getPlantTime().asSeconds() >= sf::seconds(0.75f).asSeconds()) {
						safe = false;
						break;
					}
				}
				if (right && bombTile.x + i < COLS) {
					//right
					if (map[bombTile.y][bombTile.x + i].isDestructible()) {
						right = false;
					}
					else if (!map[bombTile.y][bombTile.x + i].isWalkable()) {
						right = false;
					}
					else if (locationTile.x == bombTile.x + i &&  locationTile.y == bombTile.y && clock.getElapsedTime().asSeconds() - (*it)->getPlantTime().asSeconds() >= sf::seconds(0.75f).asSeconds()) {
						safe = false;
						break;
					}
				}
				if (down && bombTile.y + i < ROWS) {
					//down
					if (map[bombTile.y + i][bombTile.x].isDestructible()) {
						down = false;
					}
					else if (!map[bombTile.y + i][bombTile.x].isWalkable()) {
						down = false;
					}
					else if (locationTile.x == bombTile.x && locationTile.y == bombTile.y + i && clock.getElapsedTime().asSeconds() - (*it)->getPlantTime().asSeconds() >= sf::seconds(0.75f).asSeconds()) {
						safe = false;
						break;
					}
				}
				if (up && bombTile.y - i >= 0) {
					//up
					if (map[bombTile.y - i][bombTile.x].isDestructible()) {
						up = false;
					}
					else if (!map[bombTile.y - i][bombTile.x].isWalkable()) {
						up = false;
					}
					else if (locationTile.x == bombTile.x && locationTile.y == bombTile.y - i && clock.getElapsedTime().asSeconds() - (*it)->getPlantTime().asSeconds() >= sf::seconds(0.75f).asSeconds()) {
						safe = false;
						break;
					}
				}
			}
		}
		if (safe) std::cout << "safe\n";
		else
			std::cout << "not safe!!\n";
		return !safe;
	}
	~BombManager() {
		for (std::forward_list<Bomb*>::iterator it = bombsPlanted.begin();
			it != bombsPlanted.end(); it++) {
			delete *it;
		}
	}
private:
	int bombRadius, bombLimit, bombCount;
	std::forward_list<Bomb*> bombsPlanted;
	sf::Texture texture;
	sf::Clock clock;
	Tile** map;
};

class Pathfinder{
public:
	struct TileInfo{
		Tile* tile;
		int F, G, H;
		TileInfo* parent;
		bool operator==(const TileInfo &tileInfo) const  {
			return tile->getIndex() == tileInfo.tile->getIndex();
			// do whatever you need to determine whether 
			// `GameScreen` instance `other` is equal (in value) 
			// to `this` instance of `GameScreen`
		}
	};
	struct greater{
		bool operator()(const TileInfo& a, const TileInfo& b) const{
			return a.F > b.F;
		}
	};

	Pathfinder(Tile** map, Tile* start, Tile* goal) {
		this->start = start;
		this->goal = goal;
		this->map = map;
	}
	int heuristic(sf::Vector2i source, sf::Vector2i dest) {
		return (abs(dest.x - source.x) + abs(dest.y - source.y)) * 10;
	}
	std::forward_list<Tile*> AStar(BombManager &bm, BombManager &bm2) {
		std::vector<TileInfo> openList;
		std::forward_list<TileInfo> closedList;
		std::forward_list<Tile*> path;
		TileInfo* currentTile;
		TileInfo startTile;
		currentTile = &startTile;
		currentTile->tile = start;
		currentTile->G = 0;
		currentTile->H = heuristic(start->getIndex(), goal->getIndex());
		currentTile->F = currentTile->G + currentTile->H;
		currentTile->parent = NULL;
		openList.push_back(*currentTile);
		bool goalFound = false;
		while (!openList.empty()) {
			TileInfo* bestTile = &openList[0];
			int bestIdx = 0;
			int i = 0;
			for (std::vector<TileInfo>::iterator it = openList.begin(); it != openList.end(); it++) {
				if (it->F < bestTile->F) {
					bestTile = &(*it);
					bestIdx = i;
				}
				i++;
			}
			closedList.push_front(openList[bestIdx]);
			if (bestTile->tile->getIndex() == goal->getIndex()) {
				goalFound = true;
				break;
			}
			std::vector<TileInfo>::iterator it = openList.begin();
			while (it != openList.end()) {
				if (it->tile->getIndex() == bestTile->tile->getIndex()) {
					it = openList.erase(it);
				}
				else {
					++it;
				}
			}
			currentTile = &closedList.front();
			sf::Vector2i tilePos = currentTile->tile->getIndex();
			int k = rand() % 3;
			bool kFlag[3] = { true, true, true };
			bool breakFlag = false;
			while ((kFlag[0] || kFlag[1] || kFlag[2]) && !breakFlag) {
				kFlag[k] = false;
				for (int i = tilePos.y - k; i <= tilePos.y + k; i++) {
					for (int j = tilePos.x - k; j <= tilePos.x + k; j++) {
						if (i >= 0 && j >= 0 && i < ROWS && j < COLS)
						if (((i == tilePos.y - k || i == tilePos.y + k) && j == tilePos.x) || (i == tilePos.y && (j == tilePos.x - k || j == tilePos.x + k))) {
							if (map[i][j].getIndex() == goal->getIndex() && (!map[i][j].isWalkable() || map[i][j].isHoldingBomb() || bm.checkLocationForExplosions(sf::Vector2i(j, i)) || bm2.checkLocationForExplosions(sf::Vector2i(j, i)))) {
								path.clear();
								return path;
							}
							if (map[i][j].isWalkable() && !map[i][j].isHoldingBomb() && !bm.checkLocationForExplosions(sf::Vector2i(j, i)) && !bm2.checkLocationForExplosions(sf::Vector2i(j, i))) {
								TileInfo neighbour;
								neighbour.tile = &map[i][j];
								neighbour.G = currentTile->G + 10;
								neighbour.H = heuristic(neighbour.tile->getIndex(), goal->getIndex());
								neighbour.F = neighbour.G + neighbour.H;
								TileInfo* contender = NULL;
								bool found = false;
								for (std::vector<TileInfo>::iterator it = openList.begin(); it != openList.end(); it++) {
									if (it->tile->getIndex() == map[i][j].getIndex()) {
										found = true;
										contender = &(*it);
										breakFlag = true;
										break;
									}
								}
								if (found) {
									if (neighbour.F > contender->F) {
										continue;
									}
								}
								bool closed = false;
								for (std::forward_list<TileInfo>::iterator it = closedList.begin(); it != closedList.end(); it++) {
									if (it->tile->getIndex() == map[i][j].getIndex()) {
										closed = true;
										contender = &(*it);
										breakFlag = true;
										break;
									}
								}
								if (closed) {
									if (neighbour.F > contender->F) {
										continue;
									}
								}
								std::vector<TileInfo>::iterator it1 = openList.begin();
								while (found && it1 != openList.end()) {
									if (it1->tile->getIndex() == contender->tile->getIndex()) {
										it = openList.erase(it);
									}
									else {
										++it;
									}
								}
								std::forward_list<TileInfo>::iterator it2 = closedList.begin();
								while (closed && it2 != closedList.end()) {
									if (it2->tile->getIndex() == contender->tile->getIndex()) {
										closedList.remove(*it2++);
									}
									else {
										++it2;
									}
								}
								neighbour.parent = currentTile;
								openList.push_back(neighbour);
							}

						}
					}
				}
				while (!kFlag[k]) {
					k = rand() % 3;
					if (!kFlag[0] && !kFlag[1] && !kFlag[2])
						break;
				}
			}
			closedList.push_front(*currentTile);
		}
		if (!goalFound || closedList.empty()) {
			path.clear();
			return path;
		}
		TileInfo* it = &closedList.front();
		while (it) {
			path.push_front(it->tile);
			it = it->parent;
		}
		return path;
	}

	std::forward_list<Tile*> AStar() {
		std::vector<TileInfo> openList;
		std::forward_list<TileInfo> closedList;
		std::forward_list<Tile*> path;
		TileInfo* currentTile;
		TileInfo startTile;
		currentTile = &startTile;
		currentTile->tile = start;
		currentTile->G = 0;
		currentTile->H = heuristic(start->getIndex(), goal->getIndex());
		currentTile->F = currentTile->G + currentTile->H;
		currentTile->parent = NULL;
		openList.push_back(*currentTile);
		bool goalFound = false;
		while (!openList.empty()) {
			TileInfo* bestTile = &openList[0];
			int bestIdx = 0;
			int i = 0;
			for (std::vector<TileInfo>::iterator it = openList.begin(); it != openList.end(); it++) {
				if (it->F < bestTile->F) {
					bestTile = &(*it);
					bestIdx = i;
				}
				i++;
			}
			closedList.push_front(openList[bestIdx]);
			if (bestTile->tile->getIndex() == goal->getIndex()) {
				goalFound = true;
				break;
			}
			std::vector<TileInfo>::iterator it = openList.begin();
			while (it != openList.end()) {
				if (it->tile->getIndex() == bestTile->tile->getIndex()) {
					it = openList.erase(it);
				}
				else {
					++it;
				}
			}
			currentTile = &closedList.front();
			sf::Vector2i tilePos = currentTile->tile->getIndex();
			for (int i = tilePos.y - 1; i <= tilePos.y + 1; i++) {
				for (int j = tilePos.x - 1; j <= tilePos.x + 1; j++) {
					if (i >= 0 && j >= 0 && i < ROWS && j < COLS)
					if (((i == tilePos.y - 1 || i == tilePos.y + 1) && j == tilePos.x) || (i == tilePos.y && (j == tilePos.x - 1 || j == tilePos.x + 1))) {
						if (map[i][j].getIndex() == goal->getIndex() && !map[i][j].isWalkable()) {
							path.clear();
							return path;
						}
						if (map[i][j].isWalkable() && !map[i][j].isHoldingBomb()) {
							TileInfo neighbour;
							neighbour.tile = &map[i][j];
							neighbour.G = currentTile->G + 10;
							neighbour.H = heuristic(neighbour.tile->getIndex(), goal->getIndex());
							neighbour.F = neighbour.G + neighbour.H;
							TileInfo* contender = NULL;
							bool found = false;
							for (std::vector<TileInfo>::iterator it = openList.begin(); it != openList.end(); it++) {
								if (it->tile->getIndex() == map[i][j].getIndex()) {
									found = true;
									contender = &(*it);
									break;
								}
							}
							if (found) {
								if (neighbour.F > contender->F) {
									continue;
								}
							}
							bool closed = false;
							for (std::forward_list<TileInfo>::iterator it = closedList.begin(); it != closedList.end(); it++) {
								if (it->tile->getIndex() == map[i][j].getIndex()) {
									closed = true;
									contender = &(*it);
									break;
								}
							}
							if (closed) {
								if (neighbour.F > contender->F) {
									continue;
								}
							}
							std::vector<TileInfo>::iterator it1 = openList.begin();
							while (found && it1 != openList.end()) {
								if (it1->tile->getIndex() == contender->tile->getIndex()) {
									it = openList.erase(it);
								}
								else {
									++it;
								}
							}
							std::forward_list<TileInfo>::iterator it2 = closedList.begin();
							while (closed && it2 != closedList.end()) {
								if (it2->tile->getIndex() == contender->tile->getIndex()) {
									closedList.remove(*it2++);
								}
								else {
									++it2;
								}
							}
							neighbour.parent = currentTile;
							openList.push_back(neighbour);
						}

					}
				}
			}
			closedList.push_front(*currentTile);
		}
		if (!goalFound || closedList.empty()) {
			path.clear();
			return path;
		}
		TileInfo* it = &closedList.front();
		while (it) {
			path.push_front(it->tile);
			it = it->parent;
		}
		return path;
	}
private:
	Tile* start;
	Tile* goal;
	Tile** map;
};

class Enemy{
public:
	enum EnemyType{ HEnemy, VEnemy, HVEnemy, SEnemy };
	Enemy(float x, float y, sf::Texture &texture, int enemyType, int enemyDirection, int color, Bomberman &b, sf::Clock &clock)	{
		this->enemyType = enemyType;
		this->direction = enemyDirection;
		this->map = map;
		this->bomberman = &b;
		this->clock = clock;
		dead = patrol = false;
		sprite.setTexture(texture);
		sprite.setPosition(x, y);
		int frameWidth = 32, frameHeight = 32;
		sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
		for (int i = 0; i < texture.getSize().y / frameHeight; i++) {
			for (int j = color * 2; j < color * 2 + 2; j++) {
				sf::IntRect frameRect(frameWidth*j, frameHeight*i, frameWidth, frameHeight);
				if (i == 0) {
					right.addFrame(1.f, frameRect);
				}
				else if (i == 1) {
					down.addFrame(1.f, frameRect);
				}
				else if (i == 2) {
					left.addFrame(1.f, frameRect);
				}
				else if (i == 3) {
					up.addFrame(1.f, frameRect);
				}
			}
		}
		animator.addAnimation("down", down, sf::seconds(0.5f));
		animator.addAnimation("up", up, sf::seconds(0.5f));
		animator.addAnimation("left", left, sf::seconds(0.5f));
		animator.addAnimation("right", right, sf::seconds(0.5f));
	}

	void setMap(Tile** map) {
		this->map = map;
	}

	void momentOfSilence(sf::Time) {

	}

	void death() {
		dead = true;
	}

	bool isDead() {
		return dead;
	}

	int getEnemyType() {
		return enemyType;
	}

	void move() {
		if (enemyType == SEnemy) {
			if (!path.empty()) {
				patrol = false;
				sf::Vector2i currentTile = worldToTileCoord(sprite.getPosition());
				for (std::forward_list<Tile*>::iterator it = path.begin(); it != path.end(); it++) {
					if ((*it)->getIndex() == currentTile) it++;
					else continue;
					if (it == path.end()) break;
					if (static_cast<int>(sprite.getPosition().x) % 32 != 0 || static_cast<int>(sprite.getPosition().y) % 32 != 0) {
						continue;
					}
					if ((*it)->getIndex().x == currentTile.x && (*it)->getIndex().y == currentTile.y - 1 && (*it)->isWalkable()) {
						//move up
						direction = UP;
					}
					else if ((*it)->getIndex().x == currentTile.x && (*it)->getIndex().y == currentTile.y + 1 && (*it)->isWalkable()) {
						//move down					
						direction = DOWN;
					}
					else if ((*it)->getIndex().y == currentTile.y && (*it)->getIndex().x == currentTile.x - 1 && (*it)->isWalkable()) {
						//move left
						direction = LEFT;
					}
					else if ((*it)->getIndex().y == currentTile.y && (*it)->getIndex().x == currentTile.x + 1 && (*it)->isWalkable()) {
						//move right
						direction = RIGHT;
					}
					switch (direction) {
					case UP:
						velocity = sf::Vector2f(0, -ENEMY_SPEED);
						break;
					case DOWN:
						velocity = sf::Vector2f(0, ENEMY_SPEED);
						break;
					case LEFT:
						velocity = sf::Vector2f(-ENEMY_SPEED, 0);
						break;
					case RIGHT:
						velocity = sf::Vector2f(ENEMY_SPEED, 0);
						break;
					}
					break;
				}
			}
			else {
				patrol = true;
			}
			if (clock.getElapsedTime().asSeconds() - newPathTime.asSeconds() > sf::seconds(0.5f).asSeconds()) {
				newPathTime = clock.getElapsedTime();
				Pathfinder p(map, &map[getTile().y][getTile().x], &map[bomberman->getTile().y][bomberman->getTile().x]);
				path = p.AStar();
			}
		}
		sprite.setPosition(sprite.getPosition() + velocity);
	}


	void changeDirection() {
		prevDirection = direction;
		switch (enemyType) {
		case HEnemy:
			velocity = sf::Vector2f(-velocity.x, 0);
			direction = direction == LEFT ? RIGHT : LEFT;
			break;
		case VEnemy:
			velocity = sf::Vector2f(0, -velocity.y);
			direction = direction == UP ? DOWN : UP;
			break;
		case SEnemy:
			if (patrol) {
		case HVEnemy:
			direction = (direction + 1) % NUMBER_OF_DIRECTIONS;
			switch (direction) {
			case UP:
				velocity = sf::Vector2f(0, -ENEMY_SPEED);
				break;
			case DOWN:
				velocity = sf::Vector2f(0, ENEMY_SPEED);
				break;
			case LEFT:
				velocity = sf::Vector2f(-ENEMY_SPEED, 0);
				break;
			case RIGHT:
				velocity = sf::Vector2f(ENEMY_SPEED, 0);
				break;
			}
			break;
			}
		}
	}

	void setVelocity(sf::Vector2f velocity) {
		this->velocity = velocity;
	}

	sf::Vector2i getTile() {
		return worldToTileCoord(sprite.getPosition());
	}

	sf::Vector2f getVelocity() {
		return velocity;
	}

	enum Direction{ DOWN, LEFT, UP, RIGHT, NUMBER_OF_DIRECTIONS };

	void setDirection(int direction) {
		prevDirection = this->direction;
		this->direction = direction;
	}

	void draw(sf::RenderWindow& window, sf::Time dt) {
		if (prevDirection != direction) {
			switch (direction) {
			case UP:
				animator.playAnimation("up", true);
				break;
			case DOWN:
				animator.playAnimation("down", true);
				break;
			case LEFT:
				animator.playAnimation("left", true);
				break;
			case RIGHT:
				animator.playAnimation("right", true);
				break;
			}
		}
		prevDirection = direction;
		animator.update(dt);
		animator.animate(sprite);
		window.draw(sprite);
	}
	sf::Sprite& getSprite() {
		return sprite;
	}
private:
	int prevDirection, direction, enemyType;
	bool dead, halted, patrol;
	Tile** map;
	std::forward_list<Tile*> path;
	Bomberman* bomberman;
	sf::Vector2f velocity;
	sf::Sprite sprite;
	sf::Time newPathTime;
	sf::Clock clock;
	thor::FrameAnimation up, down, left, right;
	thor::Animator<sf::Sprite, std::string> animator;
};

class EnemyManager{
public:
	enum EnemyColor{ Red, Orange, Pink, Green, Purple, NumberOfColors };
	EnemyManager(const std::string &filename, Bomberman &b, sf::Clock &clock) {
		this->b = &b;
		this->clock = clock;
		texture.loadFromFile(filename);
		texture.setSmooth(true);
		currentColor = Orange;
	}

	void setMap(Tile** map) {
		for (std::forward_list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
			(*it)->setMap(map);
		}
	}

	void addEnemy(float x, float y, int enemyType, int enemyDirection, sf::Vector2f velocity) {
		int color = currentColor;
		if (currentColor == Red) currentColor = (currentColor + 1) % NumberOfColors;
		if (enemyType == Enemy::SEnemy) {
			currentColor = Red;
		}
		Enemy* enemy = new Enemy(x, y, texture, enemyType, enemyDirection, currentColor, *b, clock);
		enemy->setVelocity(velocity);
		enemies.push_front(enemy);
		currentColor = color;
		currentColor = (currentColor + 1) % NumberOfColors;
	}

	void draw(sf::RenderWindow &window, sf::Time dt) {
		for (std::forward_list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
			(*it)->draw(window, dt);
		}
	}

	std::forward_list<Enemy*>& getEnemies() {
		return enemies;
	}

	~EnemyManager() {
		for (std::forward_list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
			delete *it;
		}
	}

private:
	Bomberman* b;
	int currentColor;
	std::forward_list<Enemy*> enemies;
	sf::Texture texture;
	sf::Clock clock;
};

class TileManager{
public:
	enum TileData{ Grass, Box, Solid, HEnemy, VEnemy, HVEnemy, SEnemy };
	TileManager(const std::string mapFile, sf::Texture& drops, sf::Texture& tiles, sf::Texture& explosions, EnemyManager& em, sf::Clock& clock) {
		this->clock = clock;
		this->drops = drops;
		this->tiles = tiles;
		tiles.setSmooth(true);
		srand(time(NULL));
		map = new Tile*[ROWS];
		for (int i = 0; i < ROWS; i++) {
			map[i] = new Tile[COLS];
		}
		std::ifstream fptr(mapFile);
		std::string s1, s2;
		int i = 0, j = 0;
		int tileData[ROWS][COLS];
		int enemiesCount = 0;
		while (std::getline(fptr, s1)) {
			if (s1.find("Comment") == std::string::npos) {
				std::stringstream ss1(s1);
				while (std::getline(ss1, s2, ',')) {
					std::stringstream ss2(s2);
					ss2 >> tileData[i][j];
					/*if (i==0 || j==0 || i==ROWS-1 || j==COLS-1) {
					tileData[i][j] = Solid;
					}
					else if (enemiesCount < 5) {
					tileData[i][j] = rand()%7;
					if (tileData[i][j] > 2) enemiesCount++;
					}
					else {
					tileData[i][j] = rand()%3;
					}*/
					j++;
					if (j == COLS) j = 0;
				}
				i++;
			}
		}
		srand(time(NULL));
		explosions.setSmooth(true);
		bool portalSet = false;
		int portalX, portalY;
		while (!portalSet) {
			portalX = rand() % COLS;
			portalY = rand() % ROWS;
			if (tileData[portalY][portalX] == Box) {
				portalSet = true;
			}
		}
		portalIdx = sf::Vector2i(portalX, portalY);
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				map[i][j] = Tile(j*TILE_WIDTH, i*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, &explosions);
				map[i][j].getSprite().setTexture(tiles);
				map[i][j].getSprite().setPosition(j*TILE_WIDTH, i*TILE_HEIGHT);
				bool enemyFlag = false;
				int enemyType, enemyDirection;
				sf::Vector2f velocity(0, 0);
				switch (tileData[i][j]) {
				case HEnemy:
					velocity = sf::Vector2f(ENEMY_SPEED, 0);
					enemyFlag = true;
					enemyType = Enemy::HEnemy;
					enemyDirection = Enemy::RIGHT;
				case VEnemy:
					if (!enemyFlag) {
						velocity = sf::Vector2f(0, ENEMY_SPEED);
						enemyFlag = true;
						enemyType = Enemy::VEnemy;
						enemyDirection = Enemy::DOWN;
					}
				case HVEnemy:
					if (!enemyFlag) {
						velocity = sf::Vector2f(0, ENEMY_SPEED);
						enemyFlag = true;
						enemyType = Enemy::HVEnemy;
						enemyDirection = Enemy::DOWN;
					}
				case SEnemy:
					if (!enemyFlag) {
						velocity = sf::Vector2f(ENEMY_SPEED, 0);
						enemyType = Enemy::SEnemy;
						enemyDirection = Enemy::RIGHT;
					}
					em.addEnemy(j*TILE_WIDTH, i*TILE_HEIGHT, enemyType, enemyDirection, velocity);
				case Grass:
					map[i][j].setTileType(Tile::Grass);
					map[i][j].getSprite().setTextureRect(sf::IntRect(0, 2 * TILE_HEIGHT - 1, TILE_WIDTH, TILE_HEIGHT));
					break;
				case Box:
					if (i == portalIdx.y && j == portalIdx.x) {
						map[i][j].setTileType(Tile::HiddenPortal);
						map[i][j].getSprite().setTextureRect(sf::IntRect(0, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
						map[i][j].getSecondarySprite() = sf::Sprite();
						map[i][j].getSecondarySprite().setPosition(map[i][j].getSprite().getPosition());
						map[i][j].getSecondarySprite().setTexture(tiles);
						map[i][j].getSecondarySprite().setTextureRect(sf::IntRect(0, 3 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
						map[i][j].getSecondarySprite().setColor(sf::Color(64, 0, 0, 255));
					}
					else {
						map[i][j].setTileType(Tile::Box);
						map[i][j].getSprite().setTextureRect(sf::IntRect(0, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
					}
					break;
				case Solid:
					map[i][j].setTileType(Tile::Solid);
					map[i][j].getSprite().setTextureRect(sf::IntRect(0, 0, TILE_WIDTH, TILE_HEIGHT));
					break;
				default:
					std::cout << "Error! Invalid tile type." << std::endl;
				}
			}
		}
		portalActive = false;
	}

	enum Powers{ FireUp, FireDown, BombUp, Skate, Power, Kick, Skull, Life, Slow, NumberOfPowers };
	void draw(sf::RenderWindow& window, sf::Time dt) {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				if (map[i][j].getTileType() == Tile::Grass) {
					window.draw(map[i][j].getSprite());
					if (map[i][j].isOnFire(clock.getElapsedTime())) {
						sf::Vector2i tile = map[i][j].getIndex();
						map[i][j].getAnimator().animate(map[i][j].getSecondarySprite());
						map[i][j].getAnimator().update(dt);
						if (!map[i][j].getAnimator().isPlayingAnimation()) {
							map[i][j].getAnimator().playAnimation("explosion", true);
						}
						window.draw(map[i][j].getSecondarySprite());
					}
				}
				else if (map[i][j].getTileType() == Tile::HiddenPortal) {
					window.draw(map[i][j].getSecondarySprite());
					window.draw(map[i][j].getSprite());
				}
				else if (map[i][j].getTileType() == Tile::RevealedPortal) {
					window.draw(map[i][j].getSprite());
					window.draw(map[i][j].getSecondarySprite());
				}
				else {
					window.draw(map[i][j].getSprite());
					if (map[i][j].isOnFire(clock.getElapsedTime())) {
						if (map[i][j].isDestructible()) {
							map[i][j].setExplosionTime(sf::Time(sf::microseconds(0)));
						}
						if (map[i][j].getTileType() == Tile::Drop) {
							map[i][j].getSprite().setTexture(tiles);
						}
						map[i][j].getSprite().setTextureRect(sf::IntRect(0, 2 * TILE_HEIGHT - 1, TILE_WIDTH, TILE_HEIGHT));
						if (map[i][j].getTileType() == Tile::Box && rand() % DROP_PROBABILITY) {
							int dropType = rand() % NumberOfPowers;
							map[i][j].getSprite().setTexture(drops);
							map[i][j].receiveDrop(dropType);
							switch (dropType) {
							case FireUp:
								map[i][j].getSprite().setTextureRect(sf::IntRect(2 * TILE_WIDTH, 0, TILE_WIDTH, TILE_HEIGHT));
								break;
							case FireDown:
								map[i][j].getSprite().setTextureRect(sf::IntRect(0, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
								break;
							case BombUp:
								map[i][j].getSprite().setTextureRect(sf::IntRect(5 * TILE_WIDTH, 0, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Skate:
								map[i][j].getSprite().setTextureRect(sf::IntRect(4 * TILE_WIDTH, 0, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Power:
								map[i][j].getSprite().setTextureRect(sf::IntRect(2 * TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Kick:
								map[i][j].getSprite().setTextureRect(sf::IntRect(1 * TILE_WIDTH, 0, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Slow:
								map[i][j].getSprite().setTextureRect(sf::IntRect(4 * TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Life:
								map[i][j].getSprite().setTextureRect(sf::IntRect(5 * TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
								break;
							case Skull:
								map[i][j].getSprite().setTextureRect(sf::IntRect(0, 0, TILE_WIDTH, TILE_HEIGHT));
								break;
							default:
								std::cout << "Error! Invalid drop type." << std::endl;
							}
						}
						else map[i][j].setTileType(Tile::Grass);
					}
				}
			}
		}
	}

	void activatePortal() {
		map[portalIdx.y][portalIdx.x].getSecondarySprite().setColor(sf::Color(150, 128, 128, 255));
		portalActive = true;
	}

	bool portalIsActive() {
		return portalActive;
	}

	Tile* getPortal() {
		return &map[portalIdx.y][portalIdx.x];
	}

	sf::Texture& getTileTexture() {
		return tiles;
	}

	Tile** getMap() {
		return map;
	}

	~TileManager() {
		for (int i = 0; i < ROWS; i++) {
			delete[] map[i];
		}
		delete[] map;
	}
private:
	bool portalActive;
	sf::Vector2i portalIdx;
	sf::Clock clock;
	Tile** map;
	sf::Texture tiles, drops;
};

bool checkCollisionForBomb(Bomb* bomb, BombManager &bm, TileManager &tm) {
	sf::Vector2i prevTile = worldToTileCoord(bomb->getPrevPosition());
	sf::Vector2i currTile;
	switch (bomb->getKickDirection()) {
	case Bomb::UP:
	case Bomb::LEFT:
		currTile = worldToTileCoord(bomb->getPosition());
		break;
	case Bomb::DOWN:
		currTile = worldToTileCoord(sf::Vector2f(bomb->getPosition().x, bomb->getPosition().y + bomb->getSprite().getTextureRect().height + 12));
		break;
	case Bomb::RIGHT:
		currTile = worldToTileCoord(sf::Vector2f(bomb->getPosition().x + bomb->getSprite().getTextureRect().width + 12, bomb->getPosition().y));
		break;
	}
	if (currTile != prevTile) {
		if (!tm.getMap()[currTile.y][currTile.x].isWalkable())
			return true;
		if (tm.getMap()[currTile.y][currTile.x].isHoldingBomb())
			return true;
		currTile = worldToTileCoord(bomb->getPosition());
		if (currTile != prevTile) {
			tm.getMap()[prevTile.y][prevTile.x].removeBomb();
			tm.getMap()[currTile.y][currTile.x].receiveBomb();
		}
	}
	return false;
}

bool checkCollisionForEnemy(Enemy* enemy, BombManager &bm, TileManager &tm, sf::Time elapsedTime) {
	sf::Vector2i enemyTile = worldToTileCoord(enemy->getSprite().getPosition());
	for (int i = enemyTile.y - 1; i <= enemyTile.y + 1; i++) {
		for (int j = enemyTile.x - 1; j <= enemyTile.x + 1; j++) {
			if (i >= 0 && j >= 0 && i < ROWS && j < COLS) {
				if (!tm.getMap()[i][j].isWalkable() && tm.getMap()[i][j].getSprite().getGlobalBounds().intersects(enemy->getSprite().getGlobalBounds())) {
					return true;
				}
				if (tm.getMap()[i][j].isOnFire(elapsedTime) && tm.getMap()[i][j].getSprite().getGlobalBounds().intersects(enemy->getSprite().getGlobalBounds())) {
					enemy->death();
					return true;
				}
			}
		}
	}
	sf::FloatRect spriteRect;
	spriteRect.left = enemy->getSprite().getPosition().x + 10;
	spriteRect.width = enemy->getSprite().getTextureRect().width - 10;
	spriteRect.top = enemy->getSprite().getPosition().y + 5;
	spriteRect.height = enemy->getSprite().getTextureRect().height - 5;
	std::forward_list<Bomb*>* bombs = &bm.getBombs();
	for (std::forward_list<Bomb*>::iterator it = bombs->begin(); it != bombs->end(); it++) {
		if ((*it)->getSprite().getGlobalBounds().intersects(spriteRect)) {
			return true;
		}
	}
	return false;
}

bool checkCollisionForBomberman(Bomberman &b, BombManager &bm, TileManager &tm, bool modifyBM, bool AI, sf::Time elapsedTime) {
	if (b.isDead())
		b.respawn();
	else {
		sf::FloatRect spriteRect;
		if (AI) {
			spriteRect.left = b.getPrevPosition().x + 5;
			spriteRect.top = b.getPrevPosition().y + 42;
			spriteRect.width = b.getSprite().getTextureRect().width - 2.5;
			spriteRect.height = b.getSprite().getTextureRect().height - 48;
		}
		else {
			spriteRect.left = b.getPrevPosition().x + 5;
			spriteRect.top = b.getPrevPosition().y + 45;
			spriteRect.width = b.getSprite().getTextureRect().width - 2.5;
			spriteRect.height = b.getSprite().getTextureRect().height - 40;
		}
		sf::Vector2i tilePos = worldToTileCoord(sf::Vector2f(spriteRect.left, spriteRect.top));
		for (int i = tilePos.y - 1; i <= tilePos.y + 1; i++) {
			for (int j = tilePos.x - 1; j <= tilePos.x + 1; j++) {
				if (i >= 0 && j >= 0 && i < ROWS && j < COLS) {
					if (tm.getMap()[i][j].isOnFire(elapsedTime) && tm.getMap()[i][j].getSprite().getGlobalBounds().intersects(spriteRect)) {
						b.death();
						if (modifyBM) {
							bm.resetBombLimit();
							bm.resetBombRadius();
						}
						return true;
					}
				}
			}
		}
		spriteRect.left = b.getSprite().getPosition().x + 5;
		spriteRect.top = b.getSprite().getPosition().y + 45;
		if (modifyBM) {
			tilePos = worldToTileCoord(sf::Vector2f(spriteRect.left, spriteRect.top));
			for (int i = tilePos.y - 1; i <= tilePos.y + 1; i++) {
				for (int j = tilePos.x - 1; j <= tilePos.x + 1; j++) {
					if (i >= 0 && j >= 0 && i < ROWS && j < COLS) {
						if (!tm.getMap()[i][j].isWalkable() && tm.getMap()[i][j].getSprite().getGlobalBounds().intersects(spriteRect)) {
							b.setCollision(true);
							return true;
						}
						else if (tm.getMap()[i][j].isHoldingDrop() && tm.getMap()[i][j].getSprite().getGlobalBounds().intersects(spriteRect)) {
							tm.getMap()[i][j].setTileType(Tile::Grass);
							tm.getMap()[i][j].getSprite().setTexture(tm.getTileTexture());
							tm.getMap()[i][j].getSprite().setTextureRect(sf::IntRect(0, 2 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT));
							int dropType = tm.getMap()[i][j].getDropType();
							switch (dropType) {
							case TileManager::FireUp:
								if (bm.getBombRadius() <= MAX_BOMB_RADIUS)
									bm.setBombRadius(bm.getBombRadius() + 1);
								break;
							case TileManager::FireDown:
								if (bm.getBombRadius() > 1 && modifyBM)
									bm.setBombRadius(bm.getBombRadius() - 1);
								break;
							case TileManager::BombUp:
								if (bm.getBombLimit() <= BOMB_LIMIT && modifyBM)
									bm.setBombLimit(bm.getBombLimit() + 1);
								break;
							case TileManager::Skate:
								if (b.getSpeed() < MAX_SPEED && modifyBM) {
									b.setSpeed(b.getSpeed() + 1.f);
								}
								break;
							case TileManager::Kick:
								b.setKick(true);
								break;
							case TileManager::Power:
								bm.setBombRadius(MAX_BOMB_RADIUS);
								break;
							case TileManager::Slow:
								if (b.getSpeed() > 1.0f && modifyBM) {
									b.setSpeed(b.getSpeed() - 1.f);
								}
								break;
							case TileManager::Skull:
								b.death();
								break;
							case TileManager::Life:
								b.incrementLives();
								break;
							default:
								break;
							}
							tm.getMap()[i][j].receiveDrop(-1);
							break;
						}
					}
				}
			}
		}
		if (!sf::FloatRect(b.getLatestBombTile().x*TILE_WIDTH, b.getLatestBombTile().y*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT).intersects(spriteRect) && b.getLatestBombTile() != sf::Vector2i(-1, -1)) {
			b.setLatestBombTile(sf::Vector2i(-1, -1));
			b.setSemiGhostMode(false);
		}
		spriteRect.top = spriteRect.top - 8;
		std::forward_list<Bomb*> bombs = bm.getBombs();
		for (std::forward_list<Bomb*>::iterator it = bombs.begin(); it != bombs.end(); it++) {
			if (!b.isSemiGhost() && (*it)->getSprite().getGlobalBounds().intersects(spriteRect)) {
				if (b.canKick()) {
					(*it)->kick(b.getDirection());
				}
				return true;
			}
		}
	}
	return false;
}

void handleInput(Bomberman& bomberman, BombManager& bm, TileManager &tm, sf::Clock clock) {
	if (!bomberman.isDead()) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			bomberman.setPosition(bomberman.getPosition().x - bomberman.getSpeed(), bomberman.getPosition().y);
			bomberman.setDirection(bomberman.LEFT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			bomberman.setPosition(bomberman.getPosition().x + bomberman.getSpeed(), bomberman.getPosition().y);
			bomberman.setDirection(bomberman.RIGHT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			bomberman.setPosition(bomberman.getPosition().x, bomberman.getPosition().y - bomberman.getSpeed());
			bomberman.setDirection(bomberman.UP);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			bomberman.setPosition(bomberman.getPosition().x, bomberman.getPosition().y + bomberman.getSpeed());
			bomberman.setDirection(bomberman.DOWN);
		}
	}

	if (checkCollisionForBomberman(bomberman, bm, tm, true, false, clock.getElapsedTime())) {
		bomberman.setPosition(bomberman.getPrevPosition().x, bomberman.getPrevPosition().y);
	}
}

void handleInput(Bomberman& player1, Bomberman& player2, BombManager& bm, BombManager& bm2, TileManager &tm, Tile** map, sf::Clock clock) {
	if (!player1.isDead()) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player1.setPosition(player1.getPosition().x - player1.getSpeed(), player1.getPosition().y);
			player1.setDirection(player1.LEFT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player1.setPosition(player1.getPosition().x + player1.getSpeed(), player1.getPosition().y);
			player1.setDirection(player1.RIGHT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			player1.setPosition(player1.getPosition().x, player1.getPosition().y - player1.getSpeed());
			player1.setDirection(player1.UP);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			player1.setPosition(player1.getPosition().x, player1.getPosition().y + player1.getSpeed());
			player1.setDirection(player1.DOWN);
		}
	}

	if (checkCollisionForBomberman(player1, bm, tm, true, false, clock.getElapsedTime())) {
		player1.setPosition(player1.getPrevPosition().x, player1.getPrevPosition().y);
	}
	else if (checkCollisionForBomberman(player1, bm2, tm, false, false, clock.getElapsedTime())) {
		player1.setPosition(player1.getPrevPosition().x, player1.getPrevPosition().y);
	}

	/*if (!player2.isDead()) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		player2.setPosition(player2.getPosition().x - player2.getSpeed(), player2.getPosition().y);
		player2.setDirection(player2.LEFT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		player2.setPosition(player2.getPosition().x + player2.getSpeed(), player2.getPosition().y);
		player2.setDirection(player2.RIGHT);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		player2.setPosition(player2.getPosition().x, player2.getPosition().y - player2.getSpeed());
		player2.setDirection(player2.UP);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		player2.setPosition(player2.getPosition().x, player2.getPosition().y + player2.getSpeed());
		player2.setDirection(player2.DOWN);
		}
		}*/
	if (!player2.isDead()) {
		sf::Vector2i tilePos = player2.getTile();
		if (player2.hasCollided()) {
			player2.setCollision(false);
		}
		if (clock.getElapsedTime().asSeconds() - player2.getNewPathTime().asSeconds() > sf::seconds(0.25f).asSeconds()) {
			player2.setNewPathTime(clock.getElapsedTime());
			bool bombFlag = false;
			if (bm2.getBombCount() < bm2.getBombLimit()) {
				bm2.addBomb(sf::Vector2f(tilePos.x * TILE_WIDTH, tilePos.y * TILE_HEIGHT));
				bombFlag = true;
			}
			bool canGetToSafety = false;
			bool breakFlag = false;
			for (int k = 1; k < 3; k++) {
				for (int i = tilePos.y - k; i <= tilePos.y + k; i++) {
					for (int j = tilePos.x - k; j <= tilePos.x + k; j++) {
						if (i >= 0 && j >= 0 && i < ROWS && j < COLS)
						if (map[i][j].isWalkable() && !map[i][j].isHoldingBomb() && !bm2.checkLocationForExplosions(sf::Vector2i(j, i)) && !bm.checkLocationForExplosions(sf::Vector2i(j, i))) {
							canGetToSafety = true;
							Pathfinder p(map, &map[player2.getTile().y][player2.getTile().x], &map[i][j]);
							player2.setPathForAI(p.AStar(bm, bm2));
							player2.setNewPathTime(clock.getElapsedTime());
							player2.move();
							breakFlag = true;
							break;
						}
					}
					if (breakFlag) break;
				}
				if (breakFlag) break;
			}
			if (bombFlag && !canGetToSafety) {
				bm2.deleteBomb(sf::Vector2f(tilePos.x * TILE_WIDTH, tilePos.y * TILE_HEIGHT));
			}
			else if (bombFlag) {
				player2.setLatestBombTile(tilePos);
				player2.setSemiGhostMode(true);
			}
		}
		int p2dir = player2.getDirection();
		if (p2dir == player2.LEFT) {
			player2.setPosition(player2.getPosition().x - player2.getSpeed(), player2.getPosition().y);
		}
		else if (p2dir == player2.RIGHT) {
			player2.setPosition(player2.getPosition().x + player2.getSpeed(), player2.getPosition().y);
		}
		else if (p2dir == player2.UP) {
			player2.setPosition(player2.getPosition().x, player2.getPosition().y - player2.getSpeed());
		}
		else if (p2dir == player2.DOWN) {
			player2.setPosition(player2.getPosition().x, player2.getPosition().y + player2.getSpeed());
		}
	}
	if (checkCollisionForBomberman(player2, bm, tm, false, true, clock.getElapsedTime())) {
		player2.setPosition(player2.getPrevPosition().x, player2.getPrevPosition().y);
	}
	else if (checkCollisionForBomberman(player2, bm2, tm, true, true, clock.getElapsedTime())) {
		player2.setPosition(player2.getPrevPosition().x, player2.getPrevPosition().y);
	}
}

void SplashScreen(std::string filename, sf::RenderWindow &window, bool fadeIn, bool fadeOut) {
	sf::Texture texture;
	texture.loadFromFile(filename);
	texture.setSmooth(true);
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sf::Color color = sprite.getColor();
	int a = 0;
	if (fadeIn) {
		while (a <= 255) {
			color.a = a;
			sprite.setColor(color);
			window.draw(sprite);
			window.display();
			window.clear();
			a += 4;
		}
	}
	a = 255;
	if (fadeOut) {
		while (a >= 0) {
			sprite.setColor(color);
			window.draw(sprite);
			window.display();
			window.clear();
			a -= 4;
			color.a = a;
		}
	}
}

void action(Bomberman& player1, Bomberman& player2, BombManager& bm, BombManager& bm2, TileManager &tm, Tile** map, sf::Clock clock) {
	handleInput(player1, player2, bm, bm2, tm, map, clock);
	std::forward_list<Bomb*>* bombs = &bm.getBombs();
	std::forward_list<Bomb*>::iterator it2 = bombs->begin();
	while (it2 != bombs->end()) {
		if ((*it2)->isKicked()) {
			(*it2)->move();
			if (checkCollisionForBomb((*it2), bm, tm)) {
				(*it2)->getSprite().setPosition((*it2)->getPrevPosition());
				(*it2)->stop();
			}
		}
		++it2;
	}
	bombs = &bm2.getBombs();
	it2 = bombs->begin();
	while (it2 != bombs->end()) {
		if ((*it2)->isKicked()) {
			(*it2)->move();
			if (checkCollisionForBomb((*it2), bm, tm)) {
				(*it2)->getSprite().setPosition((*it2)->getPrevPosition());
				(*it2)->stop();
			}
		}
		++it2;
	}
}

enum Levels{
	Level1,
	Level2,
	Level3,
	Level4,
	Level5,
	NUMBER_OF_LEVELS
};

void action(Bomberman& bomberman, EnemyManager& em, BombManager& bm, TileManager &tm, int &currentLvl, sf::Clock clock) {
	handleInput(bomberman, bm, tm, clock);
	std::forward_list<Enemy*>* enemies = &em.getEnemies();
	std::forward_list<Enemy*>::iterator it = enemies->begin();
	sf::FloatRect spriteRect;
	spriteRect.left = bomberman.getSprite().getPosition().x + 5;
	spriteRect.top = bomberman.getSprite().getPosition().y + 25;
	spriteRect.width = bomberman.getSprite().getTextureRect().width - 2.5;
	spriteRect.height = bomberman.getSprite().getTextureRect().height - 20;
	while (it != enemies->end()) {
		sf::Vector2f prevPos = (*it)->getSprite().getPosition();
		(*it)->move();
		if (checkCollisionForEnemy(*it, bm, tm, clock.getElapsedTime())) {
			(*it)->getSprite().setPosition(prevPos);
			(*it)->changeDirection();
		}
		if ((*it)->getSprite().getGlobalBounds().intersects(spriteRect)) {
			if (!bomberman.isDead()) {
				bomberman.death();
			}
		}
		if ((*it)->isDead()) {
			delete *it;
			enemies->remove(*it++);
		}
		else {
			++it;
		}
	}
	std::forward_list<Bomb*>* bombs = &bm.getBombs();
	std::forward_list<Bomb*>::iterator it2 = bombs->begin();
	while (it2 != bombs->end()) {
		if ((*it2)->isKicked()) {
			(*it2)->move();
			if (checkCollisionForBomb((*it2), bm, tm)) {
				(*it2)->getSprite().setPosition((*it2)->getPrevPosition());
				(*it2)->stop();
			}
		}
		++it2;
	}

	if (!tm.portalIsActive()) {
		if (em.getEnemies().empty()) {
			tm.activatePortal();
		}
	}
	else {
		if (tm.getPortal()->getSecondarySprite().getGlobalBounds().contains(sf::Vector2f(spriteRect.left + (spriteRect.width / 2), spriteRect.top + (spriteRect.height / 2)))) {
			currentLvl++;
		}
	}
}

void drawHUD(sf::RenderWindow& window, sf::Font &font, sf::Time &elapsedTime, Bomberman &bomberman) {
	sf::Text text;
	text.setPosition(5, -35);
	text.setFont(font);
	sf::RectangleShape r;
	r.setFillColor(sf::Color::Black);
	r.setPosition(4, -35);
	r.setSize(sf::Vector2f(53, 25));
	r.setOutlineColor(sf::Color::Cyan);
	r.setOutlineThickness(1);
	window.draw(r);
	r.setPosition(300, -35);
	r.setSize(sf::Vector2f(77, 25));
	window.draw(r);
	sf::Time t = sf::seconds(180);
	t = t - elapsedTime;
	std::string mins, secs;
	mins = "3";
	if (t.asSeconds() < 180 && t.asSeconds() > 120)
		mins = "2";
	else if (t.asSeconds() < 120 && t.asSeconds() > 60) {
		mins = "1";
	}
	else if (t.asSeconds() < 60)
		mins = "0";
	if (mins == "3") {
		secs = "00";
	}
	else if (mins == "2") {
		secs = std::to_string(static_cast<int>(t.asSeconds()) - 120);
	}
	else if (mins == "1") {
		secs = std::to_string(static_cast<int>(t.asSeconds()) - 60);
	}
	else secs = std::to_string(static_cast<int>(t.asSeconds()));
	if (secs.length() == 1) {
		char temp = secs[0];
		secs[0] = '0';
		secs.push_back(temp);
	}
	text.setString("Lives: " + std::to_string(bomberman.getLives()) + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  Time: " + mins + ":" + secs);
	text.setCharacterSize(20);
	text.setColor(sf::Color::White);
	window.draw(text);
	r.setPosition(600, -35);
	r.setOutlineColor(sf::Color::Cyan);
	r.setSize(sf::Vector2f(67.5, 25));
	r.setFillColor(sf::Color::Black);
	window.draw(r);
	text.setString(" Toggle Mode");
	text.setPosition(600, -35);
	text.setCharacterSize(15);
	text.setColor(sf::Color::White);
	window.draw(text);
}

void drawHUD(sf::RenderWindow& window, sf::Font &font, sf::Time &elapsedTime, Bomberman &player1, Bomberman &player2) {
	sf::Text text;
	text.setPosition(5, -35);
	text.setFont(font);
	sf::RectangleShape r;
	r.setFillColor(sf::Color::Black);
	r.setPosition(4, -35);
	r.setSize(sf::Vector2f(77, 25));
	r.setOutlineColor(sf::Color::Cyan);
	r.setOutlineThickness(1);
	window.draw(r);
	r.setPosition(300, -35);
	r.setSize(sf::Vector2f(75, 25));
	window.draw(r);
	sf::Time t = sf::seconds(180);
	t = t - elapsedTime;
	std::string mins, secs;
	mins = "3";
	if (t.asSeconds() < 180 && t.asSeconds() > 120)
		mins = "2";
	else if (t.asSeconds() < 120 && t.asSeconds() > 60) {
		mins = "1";
	}
	else if (t.asSeconds() < 60)
		mins = "0";
	if (mins == "3") {
		secs = "00";
	}
	else if (mins == "2") {
		secs = std::to_string(static_cast<int>(t.asSeconds()) - 120);
	}
	else if (mins == "1") {
		secs = std::to_string(static_cast<int>(t.asSeconds()) - 60);
	}
	else secs = std::to_string(static_cast<int>(t.asSeconds()));
	if (secs.length() == 1) {
		char temp = secs[0];
		secs[0] = '0';
		secs.push_back(temp);
	}
	text.setString("Lives: " + std::to_string(player1.getLives()) + " - " + std::to_string(player2.getLives()) + "\t\t\t\t\t\t\t\t\t\t\t\t\t   Time: " + mins + ":" + secs);
	text.setCharacterSize(20);
	text.setColor(sf::Color::White);
	window.draw(text);
	r.setPosition(600, -35);
	r.setOutlineColor(sf::Color::Cyan);
	r.setSize(sf::Vector2f(67.5, 25));
	r.setFillColor(sf::Color::Black);
	window.draw(r);
	text.setString(" Toggle Mode");
	text.setPosition(600, -35);
	text.setCharacterSize(15);
	text.setColor(sf::Color::White);
	window.draw(text);
}

void drawSprites(sf::RenderWindow& window, sf::Font &font, Bomberman& player1, Bomberman& player2, BombManager& bm, BombManager& bm2, TileManager& tileManager, sf::Time elapsedTime, sf::Time dt) {
	drawHUD(window, font, elapsedTime, player1, player2);
	tileManager.draw(window, dt);
	player1.draw(window, dt);
	player2.draw(window, dt);
	bm.draw(window, dt);
	bm2.draw(window, dt);
	sf::FloatRect spriteRect;
	sf::RectangleShape rs;
	rs.setPosition(player2.getPosition().x + 5, player2.getPosition().y + 42);
	rs.setSize(sf::Vector2f(player2.getSprite().getTextureRect().width - 2.5, player2.getSprite().getTextureRect().height - 48));
	rs.setFillColor(sf::Color::Cyan);
	//window.draw(rs);
}

void drawSprites(sf::RenderWindow& window, sf::Font &font, Bomberman& bomberman, BombManager& bombManager, TileManager& tileManager, EnemyManager& enemyManager, sf::Time elapsedTime, sf::Time dt) {
	drawHUD(window, font, elapsedTime, bomberman);
	tileManager.draw(window, dt);
	bomberman.draw(window, dt);
	enemyManager.draw(window, dt);
	bombManager.draw(window, dt);
}

enum GameModes{
	Arcade,
	TwoPlayers,
	Battle,
	NUMBER_OF_MODES
};

int main() {
	sf::Clock clock, animationClock, timer;
	Bomberman bomberman(sf::Vector2i(1, 1), false, "bomberman.png", clock);
	Bomberman bomberman2(sf::Vector2i(19, 1), true, "bomberman2.png", clock);
	sf::View view(sf::FloatRect(0, -40, COLS*TILE_WIDTH, ROWS*TILE_HEIGHT + 40));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Bomberman", sf::Style::Close);
	sf::Font font;
	font.loadFromFile("Agencyfb.ttf");
	window.setView(view);
	window.setVerticalSyncEnabled(true);
	sf::Texture bombs, bombs2, explosion, tiles, powerups;
	bombs.loadFromFile("bombs.png");
	bombs2.loadFromFile("bombs-2.png");
	explosion.loadFromFile("explosion13.png");
	//explosion.loadFromFile("explosion5.png");
	//explosion.loadFromFile("explosion8.png");
	tiles.loadFromFile("tile2.png");
	powerups.loadFromFile("powerups.png");
	EnemyManager em("ghosts.png", bomberman, clock);
	TileManager tm("level1.txt", powerups, tiles, explosion, em, clock);
	Tile** map = tm.getMap();
	em.setMap(map);
	BombManager bm(map, bombs, clock);
	BombManager bm2(map, bombs2, clock);
	int mode = Arcade;
	int level = Levels::Level1;
	int pLvl = level;
	//SplashScreen("splash.png", window, true, true);
	//SplashScreen("splash_lvl1.png", window, true, true);
	bool restart = false;
	while (window.isOpen()) {
		restart = false;
		if (mode == Arcade) {
			pLvl = level;
		}
		sf::Event event;
		while (window.pollEvent(event))	{
			switch (event.type)	{
				// window closed
			case sf::Event::Closed:
				window.close();
				break;
				// key pressed
			case sf::Event::KeyPressed:
				break;
			case sf::Event::KeyReleased:
				if (event.key.code == sf::Keyboard::N) {
					level++;
				}
				else if (event.key.code == sf::Keyboard::B) {
					level--;
				}
				if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
					bomberman.setDirection(bomberman.IDLE);
				if (event.key.code == sf::Keyboard::Space) {
					sf::FloatRect spriteRect;
					sf::Vector2f bombermanPos;
					bombermanPos.x = bomberman.getPosition().x + 5;
					bombermanPos.x += (bomberman.getSprite().getTextureRect().width - 10) / 2;
					bombermanPos.y = bomberman.getPosition().y + 25;
					bombermanPos.y += bomberman.getSprite().getTextureRect().height - 25;
					sf::Vector2i bombTile = worldToTileCoord(bombermanPos);
					if (!bomberman.isDead() && map[bombTile.y][bombTile.x].isWalkable() && !map[bombTile.y][bombTile.x].isHoldingBomb()) {
						bm.addBomb(sf::Vector2f(bombTile.x*TILE_WIDTH, bombTile.y*TILE_HEIGHT));
						bomberman.setLatestBombTile(bombTile);
						bomberman.setSemiGhostMode(true);
					}
				}
				if (mode == TwoPlayers) {
					//	if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::A)
					//		bomberman2.setDirection(bomberman.IDLE);
					if (event.key.code == sf::Keyboard::LShift) {
						sf::FloatRect spriteRect;
						sf::Vector2f bomberman2Pos;
						bomberman2Pos.x = bomberman2.getPosition().x + 5;
						bomberman2Pos.x += (bomberman2.getSprite().getTextureRect().width - 10) / 2;
						bomberman2Pos.y = bomberman2.getPosition().y + 25;
						bomberman2Pos.y += bomberman2.getSprite().getTextureRect().height - 25;
						sf::Vector2i bombTile = worldToTileCoord(bomberman2Pos);
						if (!bomberman2.isDead() && map[bombTile.y][bombTile.x].isWalkable() && !map[bombTile.y][bombTile.x].isHoldingBomb()) {
							bm2.addBomb(sf::Vector2f(bombTile.x*TILE_WIDTH, bombTile.y*TILE_HEIGHT));
							bomberman2.setLatestBombTile(bombTile);
							bomberman2.setSemiGhostMode(true);
						}
					}
				}
				if (mode == TwoPlayers) {
					//bomberman2.setDirection(bomberman2.IDLE);
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::getPosition(window).x >= 715 && sf::Mouse::getPosition(window).x <= 795 && sf::Mouse::getPosition(window).y - 40 >= -35 && sf::Mouse::getPosition(window).y - 40 <= -10) {
					mode = (mode + 1) % NUMBER_OF_MODES;
					clock = sf::Clock();
					timer = sf::Clock();
					animationClock = sf::Clock();
					new (&bomberman) Bomberman(sf::Vector2i(1, 1), false, "bomberman.png", clock);
					new (&bomberman2) Bomberman(sf::Vector2i(19, 13), true, "bomberman2.png", clock);
					em = EnemyManager("ghosts.png", bomberman, clock);
					tm.~TileManager();
					new (&tm) TileManager("levelVS.txt", powerups, tiles, explosion, em, clock);
					map = tm.getMap();
					em.setMap(map);
					bm.~BombManager();
					bm2.~BombManager();
					new (&bm) BombManager(map, bombs, clock);
					new (&bm2) BombManager(map, bombs2, clock);
					restart = true;
				}
				break;
			default:
				break;
			}
		}
		if (timer.getElapsedTime().asSeconds() > 800 || bomberman.getLives() == 0) {
			SplashScreen("Gameover.png", window, true, false);
			window.waitEvent(event);
			while (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
				window.waitEvent(event);
			}
			if (event.key.code == sf::Keyboard::R) {
				clock = sf::Clock();
				timer = sf::Clock();
				animationClock = sf::Clock();
				new (&bomberman) Bomberman(sf::Vector2i(1, 1), false, "bomberman.png", clock);
				tm.~TileManager();
				std::string lvlFilename = "level" + std::to_string(level + 1) + ".txt";
				em = EnemyManager("ghosts.png", bomberman, clock);
				new (&tm) TileManager(lvlFilename, powerups, tiles, explosion, em, clock);
				lvlFilename = "splash_lvl" + std::to_string(level + 1) + ".png";
				SplashScreen(lvlFilename, window, true, true);
				map = tm.getMap();
				em.setMap(map);
				bm.~BombManager();
				new (&bm) BombManager(map, bombs, clock);
				continue;
			}
			else break;
		}
		if (restart) continue;
		if (mode == Arcade) {
			action(bomberman, em, bm, tm, level, clock);
			if (pLvl != level) {
				if (level == Levels::NUMBER_OF_LEVELS) {
					SplashScreen("Gameover.png", window, true, false);
					window.waitEvent(event);
					while (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
						window.waitEvent(event);
					}
					if (event.key.code == sf::Keyboard::R) {
						timer = sf::Clock();
						level = Levels::Level1;
						pLvl = Levels::Level1;
					}
					else break;
				}
				clock = sf::Clock();
				timer = sf::Clock();
				animationClock = sf::Clock();
				new (&bomberman) Bomberman(sf::Vector2i(1, 1), false, "bomberman.png", clock);
				tm.~TileManager();
				std::string lvlFilename = "level" + std::to_string(level + 1) + ".txt";
				em = EnemyManager("ghosts.png", bomberman, clock);
				new (&tm) TileManager(lvlFilename, powerups, tiles, explosion, em, clock);
				lvlFilename = "splash_lvl" + std::to_string(level + 1) + ".png";
				SplashScreen(lvlFilename, window, true, true);
				map = tm.getMap();
				em.setMap(map);
				bm.~BombManager();
				new (&bm) BombManager(map, bombs, clock);
			}
		}
		else if (mode == TwoPlayers) {
			action(bomberman, bomberman2, bm, bm2, tm, map, clock);
		}
		sf::Time dt = animationClock.restart();
		window.clear(sf::Color(60, 100, 20));
		if (mode == Arcade)
			drawSprites(window, font, bomberman, bm, tm, em, timer.getElapsedTime(), dt);
		else if (mode == TwoPlayers) {
			drawSprites(window, font, bomberman, bomberman2, bm, bm2, tm, timer.getElapsedTime(), dt);
			if (bomberman.getLives() != 0 && bomberman2.getLives() == 0) {
				SplashScreen("P1Wins.png", window, true, true);
				break;
			}
			else if (bomberman.getLives() == 0 && bomberman2.getLives() != 0) {
				SplashScreen("P2Wins.png", window, true, true);
				break;
			}
			else if (bomberman.getLives() == 0 && bomberman2.getLives() == 0) {
				SplashScreen("Draw.png", window, true, true);
				break;
			}
		}
		window.display();
	}
	return 0;
}