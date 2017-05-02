#include "SFApp.h"
//#include <SDL2/SDL_ttf.h>

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), direction('N'), deadCoins(0), score(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);


  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(((canvas_w/number_of_aliens) * i)+(canvas_w/20), canvas_h);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }



  const int number_of_walls = 2;
  for(int i= 0; i<number_of_walls; i++){

  	auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
  	auto pos   = Point2(((canvas_w/number_of_walls) * i)+(canvas_w/4), 150.0f);
  	wall->SetPosition(pos);
  	walls.push_back(wall);
  }
  const int number_of_walls2 = 3;
    for(int i= 0; i<number_of_walls2; i++){

    	auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    	auto pos   = Point2(((canvas_w/number_of_walls2) * i)+(canvas_w/6), 350.0f);
    	wall->SetPosition(pos);
    	walls.push_back(wall);
    }


  const int  number_of_coins = 4;
  for(int i = 0; i<number_of_coins; i++){
    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
   auto pos  = Point2(((canvas_w/number_of_coins) * i)+(canvas_w/8), 100.0f);
    //srand(time(0));
    //auto pos = Point2((rand() % canvas_w)+ (i*100), (rand() % canvas_h)+(i*100));
    coin->SetPosition(pos);
    coins.push_back(coin);
  }

}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    direction = 'W';
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    direction = 'E';
    break;
  case SFEVENT_PLAYER_UP:
	  player->GoNorthL();
	  direction = 'N';
	  break;
 case SFEVENT_PLAYER_DOWN:
	  player->GoSouth();
	  direction = 'S';
	  break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {


 const int points = 50;
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {

    c->GoNorthL();
  }

  // Update enemy positions
  for(auto a : aliens) {
	  if(!(a->CollidesWith(player))){
	  a->GoSouthA();
	  }

    // do something here
  }

  // Detect collisions

  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
        score=score+points;

     }
    }

	  for(auto w : walls)
    	   {
    	  if(p->CollidesWith(w)){
    		  p->HandleCollision();
    	  }
    	 }
  }
    for(auto w : walls)
   	  if(player->CollidesWith(w)){
   		player->HandlePlayerCollision(direction);
   		//player->HandleCollision();

  	  }
    for(auto a : aliens)
    	if(player->CollidesWith(a)){
    		player->HandlePlayerCollision(direction);

    		cout<<"GAME OVER "<< "\n";
    		}


for(auto c : coins)

	if (player -> CollidesWith(c)){
		c->HandleCollision();
		score = score+points;
		deadCoins++;

	}




  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);


list<shared_ptr<SFAsset>> projectile;
 for(auto p : projectiles) {
   if(p->IsAlive()) {
     projectile.push_back(p);
   }
 }
 projectiles.clear();
 projectiles= list<shared_ptr<SFAsset>>(projectile);



list<shared_ptr<SFAsset>> coin;
 for(auto c : coins) {
   if(c->IsAlive()) {
     coin.push_back(c);
   }
 }
 coins.clear();
 coins= list<shared_ptr<SFAsset>>(coin);





if(deadCoins==4){
	cout<<"YOU WON! Score: "<<score << "\n";
deadCoins= 0;
}
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto w: walls){
	  w->OnRender();
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender();}
  }


  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
