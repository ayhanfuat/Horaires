/*
 * This file is part of UMons - horaires project
 *
 *  Created on: 29/11/2013
 *      Authors: Quentin Loos, Alexandre Devaux, Jérôme Dauge
 *
 */

#include "util.h"
#include "structs/exam.h"
#include "structs/room.h"
#include "heuristics.h"
#include "conflict.h"

// This macro let you choose an example between [1,2]
#define EXAMPLE 1

// Following the macro above, defines some parameters
// concerning the chosen example
#if EXAMPLE == 1
#define MAX_TIMESLOT 5
#define MAX_EXAM 8
#elif EXAMPLE == 2
#define MAX_TIMESLOT 2
#define MAX_EXAM 2
#endif

/**
 * Make allocation for an array of students given in
 * parameters but with a variable length to be flexible.
 *
 * @param size Size of the array
 * @return An array containing all the given parameters
 */
uint32_t *init_students(int size, va_list *para) {
    int i;
    uint32_t *new_tab = calloc(size, sizeof(uint32_t));

    for (i = 0; i < size; i++)
        new_tab[i] = va_arg(*para, uint32_t);

    return new_tab;
}

/**
 * Make allocation for an array of availabilities given in
 * parameters but with a variable length to be flexible.
 *
 * @param size Size of the array
 * @return An array containing all the given parameters
 */
uint8_t *init_availabilities(int size, va_list *para) {
    int i;
    uint8_t *new_tab = calloc(size, sizeof(uint8_t));

    for (i = 0; i < size; i++)
        new_tab[i] = (uint8_t) va_arg(*para, uint32_t);

    return new_tab;
}

/**
 * Make allocation for an array of conflicts initialized to 0.
 *
 * @param size Size of the array
 * @return An array containing 0s.
 */
uint16_t *init_conflicts(int size) {
    return calloc(size, sizeof(uint16_t));
}

/**
 * Make the allocation and initialization of an exam. The parameters are
 * length variable, to have this function dynamic. It takes, in this order,
 * the 'exam id', the 'teacher id', the 'number of enrollment' followed by
 * the specified number of enrollment, the 'number of availabilities'
 * followed by the specified number of availabilities and the 'number of
 * exams'.
 *
 * @param id the exam id.
 * @return a struct exam allocated and initialized.
 */
exam *init_exam(uint16_t id, ...) {
    va_list para;
    va_start(para, id);

    exam *exam_ = calloc(1, sizeof(exam));
    exam_->exam_id    = id;
    exam_->teacher_id = (uint32_t) va_arg(para, int);

    exam_->timeslot = NOT_SCHEDULED; // Default value for non-assigned exam

    exam_->enrollment = (uint16_t) va_arg(para, int);
    // problem with the two lines below
    exam_->students = init_students(exam_->enrollment, &para);

    exam_->availabilities = init_availabilities(va_arg(para, int), &para);

    exam_->conflicts = init_conflicts(va_arg(para, int));

    return exam_;
}

/**
 *
 * Make allocation and initialization of a room with the specified
 * parameters.
 *
 * @param id the room ID.
 * @param type the room type.
 * @param capacity the capacity of the room.
 * @param faculty the faculty in charge of the room.
 * @return a struct room allocated and initialized.
 */
room *init_room(uint16_t id, room_type type, uint16_t capacity, uint8_t faculty) {
    room *room_ = calloc(1, sizeof(room));

    room_->room_id     = id;
    room_->type        = type;
    room_->capacity    = capacity;
    room_->faculty     = faculty;
    room_->assignation = calloc(MAX_TIMESLOT, sizeof(uint16_t));

    int i;

    for (i = 0; i < MAX_TIMESLOT; i++)
        room_->assignation[i] = -1;

    return room_;
}

/**
 * Make allocation for an array of exams given in parameters
 * but with a variable length to be flexible.
 *
 * @param size Size of the array to alloc.
 * @return An array containing all the given parameters.
 */
exam *init_exams(int size, ...) {
    exam *exams = calloc(size, sizeof(exam));

    va_list para;
    va_start(para, size);

    int i;
    for (i = 0; i < size; i++)
        exams[i] = *va_arg(para, exam *);

    va_end(para);

    return exams;
}

/**
 * Make allocation for an array of rooms given in parameters
 * but with a variable length to be flexible.
 *
 * @param size Size of the array to alloc.
 * @return An array containing all the given parameters.
 */
room *init_rooms(int size, ...) {
    room *rooms = calloc(size, sizeof(room));

    va_list para;
    va_start(para, size);

    int i;
    for(i = 0; i < size; i++)
        rooms[i] = *va_arg(para, room *);

    va_end(para);

    return rooms;
}

/**
 * Creates and initializes an problem instance, having a solution.
 *
 * @return An array of exams
 */
exam *get_example1() {
    // exam1 - Analyse
    exam *exam1 = init_exam(1, 555000,                      // exam id, teacher id
                            3, 10000, 10001, 10002,         // nb + enrollments
                            MAX_TIMESLOT, 1, 1, 1, 0, 0, 0, // nb + availabilities
                            MAX_EXAM);                      // nb of exams

    // exam2 - Fonctionnement des ordis
    exam *exam2 = init_exam(2, 555001,
                            2, 10000, 10003,
                            MAX_TIMESLOT, 0, 1, 1, 0, 0, 0,
                            MAX_EXAM);

    // exam3 - MATH1
    exam *exam3 = init_exam(3, 555002,
                            2, 10002, 10004,
                            MAX_TIMESLOT, 0, 1, 1, 1, 0, 0,
                            MAX_EXAM);

    // exam4 - Anglais
    exam *exam4 = init_exam(4, 555003,
                            4, 10003, 10004, 10005, 10006,
                            MAX_TIMESLOT, 1, 0, 1, 0, 0, 0,
                            MAX_EXAM);

    // exam5 - Anglais
    exam *exam5 = init_exam(5, 555001,
                            3, 10000, 10001, 10003,
                            MAX_TIMESLOT, 0, 0, 1, 1, 0, 0,
                            MAX_EXAM);

    // exam6 - chimir
    exam *exam6 = init_exam(6, 555004,
                            1, 10004,
                            MAX_TIMESLOT, 0, 0, 0, 1, 1, 1,
                            MAX_EXAM);

    // exam7 - algèbre
    exam *exam7 = init_exam(7, 555005,
                            1, 10001,
                            MAX_TIMESLOT, 0, 0, 0, 0, 1, 1,
                            MAX_EXAM);
    // exam8 - jesaispaslire
    exam *exam8 = init_exam(8, 555006,
                            1, 10002,
                            MAX_TIMESLOT, 1, 1, 0, 0, 1, 1,
                            MAX_EXAM);

    return init_exams(MAX_EXAM, exam1, exam2, exam3, exam4, exam5, exam6, exam7,
                      exam8);
}

/**
 * Creates and initializes an problem instance, having a solution.
 *
 * @return An array of exams
 */
exam *get_example2() {
    // exam1 - Analyse
    exam *exam1 = init_exam(1, 555000,                  // exam id, teacher id
                            3, 10000, 10001, 10002,    // nb + enrollments
                            2, 1, 0,                    // nb + availabilities
                            MAX_EXAM);           // nb of exams

    // exam2 - Fonctionnement des ordis
    exam *exam2 = init_exam(2, 555001,
                            2, 10000, 10003,
                            MAX_TIMESLOT, 1, 1,
                            MAX_EXAM);

    return init_exams(MAX_EXAM, exam1, exam2);
}

/**
 * Calls the right example following the macro EXAMPLE.
 *
 * @return An array of exams
 */
exam *get_example() {
    switch (EXAMPLE) {
        case 1:
            return get_example1();

        case 2:
            return get_example2();
    }

    return NULL;
}

/**
 * Create an array of rooms to use with the two first example.
 *
 * @return An array of rooms
 */
room *get_rooms() {
    // R1 - Salon bleu
    room *room1 = init_room(0, class, 1, 0);
    // R2 - Plisnier
    room *room2 = init_room(1, class, 2, 0);
    // R3 - Van Gogh
    room *room3 = init_room(2, amphitheater, 5, 0);
    // R4 - Pascal
    room *room4 = init_room(3, lab, 4, 0);

    return init_rooms(4, room3, room1, room2, room4);
}

uint16_t *get_room_type_indices(int size, room *rooms) {
    uint16_t *indices = calloc(size, sizeof(uint16_t));

    int i = 0, j = 0;
    for(i; i < size; i++) {
        if(rooms[i].type != j) {
            indices[j] = i;
            j++;
        }
    }

    while(j < 4) {
        indices[j] = i;
        j++;
    }

    return indices;
}

/**
 * Prints which timeslot have been attributed to each exams.
 *
 * @param exams An array of scheduled exams
 */
void print_summary_schedule(exam *exams) {
    int i;

    printf("Summary\n");
    printf("=======\n");

    for (i = 0; i < MAX_EXAM; i++) {
        printf("Exam %d : %d\n", i + 1, exams[i].timeslot);
    }

    printf("\n");
}

/**
 * Prints which detailed information from each exams,
 * for each timeslot available.
 *
 * @param exams An array of scheduled exams
 */
void print_detailed_schedule(exam *exams) {
    int i, j, k;

    printf("Detailed schedule\n");
    printf("=================\n");

    for (i = 0; i < MAX_TIMESLOT; i++) {
        printf("Timeslot %d\n", i + 1);
        printf("------------\n\n");

        for (j = 0; j < MAX_EXAM; j++) {
            if (exams[j].timeslot == i) {
                printf("  Exam %d :\n", j + 1);
                printf("      -> Prof : %d\n", exams[j].teacher_id);

                printf("      -> Timeslots available : (");

                for (k = 0; k < MAX_TIMESLOT; k++) {
                    printf("%d ", exams[j].availabilities[k]);
                }

                printf(")\n");

                printf("      -> Conflicts detected : (");

                for (k = 0; k < MAX_EXAM; k++) {
                    printf("%d ", exams[j].conflicts[k]);
                }

                printf(")\n");

                printf("      -> Students :\n");

                for (k = 0; k < exams[j].enrollment; k++) {
                    printf("            %d\n", exams[j].students[k]);
                }
            }
        }
    }
}

/**
 * Main function, execute the heuristics on a simple example.
 *
 */
int main() {
    // Collect a sample of exams
    exam *exams = get_example();

    room *rooms = get_rooms();
    uint16_t *indices = get_room_type_indices(4, rooms);

    // Preprocessing to the coloring graph heuristics
    compute_conflicts(exams, MAX_EXAM);

    // Main heuristic
    bool a = color_graph_backtrack(exams, MAX_EXAM, rooms, indices, MAX_TIMESLOT);

    printf("%s\n", (a == true) ? "A schedule has been found!\n" :
           "No schedule has been found!\n");

    // Some outputs
    if (a == true) {
        print_summary_schedule(exams);

        print_detailed_schedule(exams);
    }

    return 0;
}
