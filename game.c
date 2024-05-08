#include <stdio.h>
#include "include/raylib.h"
#include "include/interface.h"
#include "include/state.h"

State state;

KeyState keys_init(){
	KeyState keys=malloc(sizeof(*keys));
	if(GetKeyPressed()==KEY_UP)keys->up= true;
	else keys->up=false;
	if(GetKeyPressed()==KEY_RIGHT)keys->right= true;
	else keys->right=false;
	if(GetKeyPressed()==KEY_LEFT)keys->left= true;
	else keys->left=false;
	if(GetKeyPressed()==KEY_P)keys->p= true;
	else keys->p=false;
	if(GetKeyPressed()==KEY_N)keys->n= true;
	else keys->n=false;
	if(GetKeyPressed()==KEY_SPACE)keys->space= true;
	else keys->space=false;
	if(GetKeyPressed()==KEY_ENTER)keys->enter= true;
	else keys->enter=false;
	return keys;
}

void update_and_draw() {
	KeyState keys=keys_init;
	state_update(state,keys);
	interface_draw_frame(state);
	free(keys);
}

int main(){
	state =state_create();
	interface_init();

	start_main_loop(update_and_draw);
	interface_close();
	return 0;
}