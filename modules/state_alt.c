#include <stdlib.h>

#include "ADTSet.h"
#include "ADTList.h"
#include "state.h"
#include "vec2.h"

int compare(Pointer a, Pointer b){
    if( *(int*)a < *(int*)b )
   return -1;           // σύγκριση τιμών ως int
else if( *(int*)a > *(int*)b )
   return 1;
else if (a < b)
   return -1;     // ιδιες τιμές, ελέγχνουμε τους pointers!
else if (a > b)
   return 1;
else
   return 0;     // σημαντικό να επιστρέφει 0 όταν οι poitners είναι ίδιοι!
}

struct state {
	Set objects_x;			// περιέχει στοιχεία Object ταξινομημένα κατά x (αστεροειδείς, σφαίρες)
    Set objects_y;			// περιέχει στοιχεία Object ταξινομημένα κατά y (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, Vector2 position, Vector2 speed, Vector2 orientation, double size) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->position = position;
	obj->speed = speed;
	obj->orientation = orientation;
	obj->size = size;
	return obj;
}

// Επιστρέφει έναν τυχαίο πραγματικό αριθμό στο διάστημα [min,max]

static double randf(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

// Προσθέτει num αστεροειδείς στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα καρτεσιανό επίπεδο.
// - Η αρχή των αξόνων είναι η θέση του διαστημόπλοιου στην αρχή του παιχνιδιού
// - Στο άξονα x οι συντεταγμένες μεγαλώνουν προς τα δεξιά.
// - Στον άξονα y οι συντεταγμένες μεγαλώνουν προς τα πάνω.

static void add_asteroids(State state, int num) {
	for (int i = 0; i < num; i++) {
		// Τυχαία θέση σε απόσταση [ASTEROID_MIN_DIST, ASTEROID_MAX_DIST]
		// από το διστημόπλοιο.
		//
		Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ASTEROID_MIN_DIST, ASTEROID_MAX_DIST),	// απόσταση
				randf(0, 2*PI)									// κατεύθυνση
			)
		);

		// Τυχαία ταχύτητα στο διάστημα [ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED]
		// με τυχαία κατεύθυνση.
		//
		Vector2 speed = vec2_from_polar(
			randf(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED) * state->speed_factor,
			randf(0, 2*PI)
		);

		Object asteroid = create_object(
			ASTEROID,
			position,
			speed,
			(Vector2){0, 0},								// δεν χρησιμοποιείται για αστεροειδείς
			randf(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE)		// τυχαίο μέγεθος
		);
		set_insert(state->objects_x, asteroid);
        set_insert(state->objects_y, asteroid);
	}
}
//Δημιουργεί δύο μισούς αστεροιεδής από τον αρχικό με το μισό μέγεθος,
//τυχαία κατεύθυνση και ταχύτητα 1,5 φορά μεγαλύτερη
//Είναι τύπου Object ώστε αφού καταστραφούν ο αρχικός αστεροειδής και η σφαίρα
//να μπορεί να προστεθεί στο vector με τα υπόλοιιπα αντικείμενα

Object add_half_asteroids(State state, Object big_asteroid){
	Vector2 position= state->info.spaceship->position;
	Vector2 speed=vec2_from_polar(big_asteroid->speed.x,randf(0,2*PI));
	Object half_asteroid= create_object(ASTEROID,position,speed,(Vector2){0,0},big_asteroid->size/2);
	return half_asteroid;
}

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects_x = set_create(compare,free);
    state->objects_y = set_create(compare,free);

	// Δημιουργούμε το διαστημόπλοιο
	state->info.spaceship = create_object(
		SPACESHIP,
		(Vector2){0, 0},			// αρχική θέση στην αρχή των αξόνων
		(Vector2){0, 0},			// μηδενική αρχική ταχύτητα
		(Vector2){0, 1},			// κοιτάει προς τα πάνω
		SPACESHIP_SIZE				// μέγεθος
	);

	// Προσθήκη αρχικών αστεροειδών
	add_asteroids(state, ASTEROID_NUM);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	//Δημιουργία του state_information
	StateInfo state_information=malloc(sizeof(*state_info));
	
	//Προσθέτω τις γενικές πληροφορίες
	state_information->paused=state->info.paused;
	state_information->score=state->info.score;
	state_information->spaceship=state->info.spaceship;
	return state_information;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η θέση position βρίσκεται εντός του παραλληλογράμμου με πάνω αριστερή
// γωνία top_left και κάτω δεξιά bottom_right.

List state_objects(State state, Vector2 top_left, Vector2 bottom_right){
    List list_objects_x=list_create(free);
    ListNode list_node_x= LIST_BOF;
}