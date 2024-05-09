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
Vector2 top_left={0,SCREEN_HEIGHT};
Vector2 bottom_right={SCREEN_WIDTH,0};
//Φτιάχνουμε το translation vector
Vector2 screen_vector={(top_left.x+bottom_right.x)/2, (top_left.y+bottom_right.y)/2};
Vector2 translation_vector=vec2_add(screen_vector,vec2_scale(state_info(state)->spaceship->position,-1));

    //Φτιάχνουμε τα vectors που οριοθετούν την οθόνη
    //Η οθόνη ακολουθεί το διστημόπλοιο οπότε οι συντεταγμένες των αντικειμέων μένουν ως έχουν
    // Vector2 top_left={0,SCREEN_HEIGHT};
    // top_left=vec2_add(top_left,state_info(state)->spaceship->position);
    // top_left.y*=-1;
    // Vector2 bottom_right={SCREEN_WIDTH,0};
    // bottom_right=vec2_add(bottom_right,state_info(state)->spaceship->position);
    // bottom_right.y*=-1;

    // Σχεδιάζουμε το διαστημόπλοιο, τους αστεροειδής και τις σφαίρες μέσω της state_objects()
    List objects_on_screen = state_objects(state, top_left, bottom_right);
    for(ListNode listnode=list_first(objects_on_screen); listnode!=LIST_EOF; listnode=list_next(objects_on_screen,listnode)){
        Object object=list_node_value(objects_on_screen,listnode);
        screen_vector=vec2_add(object->position,translation_vector);
        if(object->type == ASTEROID){
            DrawCircle(object->position.x + translation_vector.x, -object->position.y-translation_vector.y, object->size, BROWN);//για αστεροειδής
        }
        else if(object->type == BULLET){
            DrawCircle(object->position.x + translation_vector.y, -object->position.y-translation_vector.y, object->size, WHITE);
        }
    }
    //Σχεδιασμός διαστημοπλοίου
    // Vector2 thirty_pixels={30,30};
    // Vector2 spaceship_position=state_info(state)->spaceship->position;
    // spaceship_position.y*=-1;
    // Vector2 starting_pos1=vec2_add(spaceship_position,thirty_pixels);
    // DrawLineV(starting_pos1,spaceship_position, BLUE);
    // vec2_scale(thirty_pixels,-1);
    // Vector2 starting_pos2=vec2_add(spaceship_position,thirty_pixels);
    // DrawLineV(starting_pos2,spaceship_position, BLUE);
    // DrawLineV(starting_pos1,starting_pos2,BLUE);
    
    DrawPoly(state_info(state)->spaceship->position, 3, SPACESHIP_SIZE, 0.0, WHITE);

    // Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", state_info(state)->score), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	if (state_info(state)->paused) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 SCREEN_WIDTH / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 SCREEN_HEIGHT / 2 - 50, 20, GRAY
		);
	}
}