
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include "vec2.h"

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.
struct state {
	Vector objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
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
		vector_insert_last(state->objects, asteroid);
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

//Προσθέτει μία σφαίρα στο παιχνίδι

static void add_bullet(State state){
	Vector2 speed=vec2_add(state_info(state)->spaceship->speed, vec2_scale(state_info(state)->spaceship->orientation, BULLET_SPEED));
	Vector2 position= state_info(state)->spaceship->position;
	Vector2 orientation= state_info(state)->spaceship->orientation;
	double size=BULLET_SIZE;
	Object bullet = create_object(BULLET, position, speed, orientation, size);
	vector_insert_last(state->objects, bullet);
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects = vector_create(0, NULL);

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

List state_objects(State state, Vector2 top_left, Vector2 bottom_right) {
	List list_objects=list_create(free);
	ListNode list_node= LIST_BOF;
	for(VectorNode vectornode= vector_first(state->objects); vectornode!=VECTOR_EOF; vectornode=vector_next(state->objects, vectornode)){
		Object helpful=vector_node_value(state->objects, vectornode);
		if(helpful->position.x>top_left.x && helpful->position.y<top_left.y && helpful->position.x<bottom_right.x && helpful->position.y>bottom_right.y ){
			list_insert_next(list_objects, list_node, helpful);
			list_node=list_last(list_objects);
		}

	}
	
	return list_objects;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {

	//Παύση και διακοπή
	if(keys->p){
		state_info(state)->paused= !state_info(state)->paused; //κάνει switch την τιμή της προηγούμενης τιμής το paused
	}
	if(state_info(state)->paused && !keys->n)//Αν το παιχνίδι βρίσκεται σε pause και δεν είναι πατημένο το n τερματίζει η συνάρτηση
		return;

	//ΕΝΑΡΞΗ
	state->next_bullet++;
	// Κίνηση αντικειμένων
	for(VectorNode vectornode= vector_first(state->objects); vectornode!=VECTOR_EOF; vectornode=vector_next(state->objects, vectornode)){
		Object helpful=vector_node_value(state->objects, vectornode);
		helpful->position = vec2_add(helpful->position,vec2_scale(helpful->speed,state->speed_factor));//
	}

	//Περιστροφή διαστημοπλοίου
	if(keys->left)
		state_info(state)->spaceship->orientation = vec2_rotate(state_info(state)->spaceship->orientation, SPACESHIP_ROTATION);
	else if(keys->right)
		state_info(state)->spaceship->orientation = vec2_rotate(state_info(state)->spaceship->orientation, -SPACESHIP_ROTATION);

	//Επιτάχυνση διαστημοπλοίου
	if(keys->up)
	state_info(state)->spaceship->speed = vec2_add(state_info(state)->spaceship->speed, vec2_scale(state_info(state)->spaceship->orientation, SPACESHIP_ACCELERATION));
	//Επιβράδυνση διαστημοπλοίου
	else if(!keys->up && state_info(state)->spaceship->speed.x/state_info(state)->spaceship->speed.y > 0)//για να μην γίνεται επ' αόριστον
	state_info(state)->spaceship->speed = vec2_scale(state_info(state)->spaceship->speed, SPACESHIP_SLOWDOWN);
	
	//Έλεγχος και δημιουργία αστεροειδών
	int asteroid_counter=0;
	for(VectorNode vectornode= vector_first(state->objects); vectornode!=VECTOR_EOF; vectornode=vector_next(state->objects, vectornode)){
		Object helpful=vector_node_value(state->objects, vectornode);
		if(helpful->type == ASTEROID){
			double distance =vec2_distance(state_info(state)->spaceship->position, helpful ->position);
			if (distance <= ASTEROID_MAX_DIST)
				asteroid_counter++;
		}
	}
	if(asteroid_counter< ASTEROID_NUM){
		add_asteroids(state, ASTEROID_NUM-asteroid_counter);
		state_info(state)->score+=1;
	}
	//Αν το σκορ είναι πολλαπλάσιο του 100 αυξάνω το speed factor κατά 10%
	if(state_info(state)->score%100 == 0)
		state->speed_factor*=1.1;
	
	//Προσθήκη σφαίρας
	if(state->next_bullet >= BULLET_DELAY && keys->space)
		add_bullet(state);

	//Συγκρούσεις

	//Πρώτα τσεκάρω για αστεροειδή με διαστημόπλοιο
	int nodecounter_for_spaceship=0;
	for(VectorNode vectornode= vector_first(state->objects); vectornode!=VECTOR_EOF; vectornode=vector_next(state->objects, vectornode)){
		Object helpful=vector_node_value(state->objects, vectornode);
		nodecounter_for_spaceship++;
		if(helpful->type == ASTEROID && CheckCollisionCircles(state_info(state)->spaceship->position,SPACESHIP_SIZE,helpful->position,helpful->size)){
			state_info(state)->score/=2;//χάνεται το μισό σκορ
			Object temp=vector_node_value(state->objects,vectornode);//αντιγράφω το περιεχόμενο του τελευταίου κόμβου του vector σε αυτόν που βρίσκεται ο αστεροειδής
			vector_set_at(state->objects,nodecounter_for_spaceship,temp);
			vector_remove_last(state->objects);//στη συνεχεια διγράφω το τελευταίο στοιχείο
			free(temp);
		}
	}

	//Τσεκάρω για αστεροειδή με σφαίρα
	int nodecounter_for_bullet=0;
	for(VectorNode vectornode= vector_first(state->objects); vectornode!=VECTOR_EOF; vectornode=vector_next(state->objects,vectornode)){
		Object helpful=vector_node_value(state->objects, vectornode);
		nodecounter_for_bullet++;
		int nodecounter_for_asteroid=0;
		if(helpful->type==BULLET)
			for(VectorNode vectornode2= vector_first(state->objects); vectornode2!=VECTOR_EOF; vectornode2=vector_next(state->objects,vectornode2)){
				Object helpful2=vector_node_value(state->objects, vectornode2);
				nodecounter_for_asteroid++;
				if(helpful2->type==ASTEROID && CheckCollisionCircles(helpful->position,helpful->size,helpful2->position,helpful2->size)){
					//Δημιουργούμε τους δύο νέους αστεροειδείς που θα προκύψουν μετά τη σύγκρουση
					bool flag=false;
					Object new_asteroid1, new_asteroid2;
					if(helpful2->size/2>ASTEROID_MIN_SIZE){
					new_asteroid1=add_half_asteroids(state, helpful2);
					new_asteroid2=add_half_asteroids(state, helpful2);
					flag = true;
					}
					//Αφαιρούμε σφαίρα και αστεροειδή					
					//Σε περίπτωση που η σφαίρα είναι ο τελευταίος κόμβος του vector θα πρέπει να αφαιρεθεί πρίν τον αστεροειδή
					if(nodecounter_for_bullet==vector_size(state->objects)){
						//διαγράφω τη σφαίρα που συγκρούστηκε
						Object temp=vector_node_value(state->objects,vectornode);//αντιγράφω το περιεχόμενο του τελευταίου κόμβου του vector σε αυτόν που βρίσκεται η σφαίρα
						vector_set_at(state->objects,nodecounter_for_bullet,temp);
						vector_remove_last(state->objects);//στη συνεχεια διγράφω το τελευταίο στοιχείο
						//όμοια για τον αστεροειδή
						temp=vector_node_value(state->objects,vectornode2);//αντιγράφω το περιεχόμενο του τελευταίου κόμβου του vector σε αυτόν που βρίσκεται ο αστεροειδής
						vector_set_at(state->objects,nodecounter_for_asteroid,temp);
						vector_remove_last(state->objects);//στη συνεχεια διαγράφω το τελευταίο στοιχείο
						free(temp);
					}
					else{
						//διαγράφω τον αστεροειδή που συγκρούστηκε
						Object temp=vector_node_value(state->objects,vectornode2);//αντιγράφω το περιεχόμενο του τελευταίου κόμβου του vector σε αυτόν που βρίσκεται ο αστεροειδής
						vector_set_at(state->objects,nodecounter_for_asteroid,temp);
						vector_remove_last(state->objects);//στη συνεχεια διαγράφω το τελευταίο στοιχείο
						//όμοια για την σφαίρα
						temp=vector_node_value(state->objects,vectornode);//αντιγράφω το περιεχόμενο του τελευταίου κόμβου του vector σε αυτόν που βρίσκεται η σφαίρα
						vector_set_at(state->objects,nodecounter_for_bullet,temp);
						vector_remove_last(state->objects);//στη συνεχεια διγράφω το τελευταίο στοιχείο
						free(temp);
					}
					//Προσθέτουμε τους αστεροειδείς στο τέλος του vector
					if(flag==true){
						vector_insert_last(state->objects, new_asteroid1);
						vector_insert_last(state->objects, new_asteroid2);
					}else{
						free(new_asteroid1);
						free(new_asteroid2);
					}


					//μειώνω το σκορ κατά 10
					state_info(state)->score-=10;
					}
		}
	}

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}