#include "raylib.h"
#include "vec2.h"
#include "state.h"
#include "interface.h"

// Assets
Sound Stargate_opening;

void interface_init(){
    // Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
    InitAudioDevice();

    // Φόρτωση ήχων
	Stargate_opening= LoadSound("../assets/Stargate_Movie_1994_opening.mp3");
}
void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}
// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();
    // Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(BLACK);

    //Φτιάχνουμε τα vectors που οριοθετούν την οθόνη
    //Η οθόνη ακολουθεί το διστημόπλοιο οπότε οι συντεταγμένες των αντικειμέων μένουν ως έχουν
    Vector2 top_left={0,SCREEN_HEIGHT};
    top_left=vec2_add(top_left,state_info(state)->spaceship->position);
    top_left.y*=-1;
    Vector2 bottom_right;
    bottom_right=vec2_add(bottom_right,state_info(state)->spaceship->position);
    bottom_right.y*=-1;

    // Σχεδιάζουμε το διαστημόπλοιο, τους αστεροειδής και τις σφαίρες μέσω της state_objects()
    List objects_on_screen = state_objects(state, top_left, bottom_right);
    ListNode listnode=list_first(objects_on_screen);
    for(listnode; listnode!=LIST_EOF; listnode=list_next(objects_on_screen,listnode)){
        Object object=list_node_value(objects_on_screen,listnode);
        if(object->type == ASTEROID){
            DrawCircle(object->position.x, -object->position.y, object->size, BROWN);//για αστεροειδής
        }
        else if(object->type == BULLET){
            DrawCircle(object->position.x, -object->position.y, object->size, WHITE);
        }
    }
    
}