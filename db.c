// This file requires at least C99 to compile

/**
 * @file   db.c
 * @author Merlin Nimier-David <merlin.nimier-david@epfl.ch>
 * @author Jean-Cédric Chappelier <jean-cedric.chappelier@epfl.ch>
 *
 * @copyright EPFL 2020
**/
/**
 * @section DESCRIPTION
 *
 * Template du homework du cours CS-207, année 2020.
**/

#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS/FAILURE
#include <math.h>   // fabs()
#include <string.h> // memset()
#include <stdint.h> // uint32_t
#include <inttypes.h> // PRIu32 & SCNu32

// ----------------------------------------------
//   ___             _            _
//  / __|___ _ _  __| |_ __ _ _ _| |_ ___
// | (__/ _ \ ' \(_-<  _/ _` | ' \  _(_-<
//  \___\___/_||_/__/\__\__,_|_||_\__/__/

#define DB_MAX_SIZE    20u
#define QUERY_MAX_SIZE  5u

// ----------------------------------------------
//  _____
// |_   _|  _ _ __  ___ ___
//   | || || | '_ \/ -_|_-<
//   |_| \_, | .__/\___/__/
//       |__/|_|

/* Définissez ici les types demandés :
 *    StudentKind,
 *    SCIPER,
 *    Student,
 *    Database,
 * et QueryResult.
 */
 enum StudentKind {Bachelor, Master, Exchange};
 typedef enum StudentKind StudentKind;
 size_t const StudentKindCount = Exchange + 1;
 typedef uint32_t SCIPER;
 typedef struct Student{
	 SCIPER sciper;
	 double grade_sn;
	 double grade_hw;
	 double grade_exam;
	 StudentKind type; 
	 const struct Student* teammate;
 } Student;
 
 typedef Student Database[DB_MAX_SIZE];
 typedef const Student* QueryResult[QUERY_MAX_SIZE];

// ----------------------------------------------
//   ___               _
//  / _ \ _  _ ___ _ _(_)___ ___
// | (_) | || / -_) '_| / -_|_-<
//  \__\_\\_,_\___|_| |_\___/__/

size_t db_entry_count(const Database db)
{   
	size_t count = 0;
	while(count<DB_MAX_SIZE && db[count].sciper != 0){
	   count++;
	};
    return count;
}

// ----------------------------------------------
const Student* get_student_by_sciper(const Database db, SCIPER sciper)
{
	if(sciper == 0) return NULL;
	
	for(int i=0; i< DB_MAX_SIZE; i++){
		if(db[i].sciper == sciper){
			return &db[i];
		}
	}
    return NULL;
}

// ----------------------------------------------
int check_teammates_consistency(const Database db)
{
	for(int i=0; i<DB_MAX_SIZE; i++){
		const Student* teammate = db[i].teammate;
		if(teammate != NULL){
			const Student* teammateOfTeammate = (*teammate).teammate;
		    SCIPER sciper1 = db[i].sciper;
			SCIPER sciper2 = (*teammate).sciper;
			if(teammateOfTeammate == NULL){
				printf("%d a %d comme binome mais %d n'a pas de binome dans la base de donnée\n",
				       sciper1, sciper2,sciper2);
				return -1;
			}else{
			    SCIPER sciper3 = (*teammateOfTeammate).sciper;
			    if(sciper1 != sciper3){
				 printf("%d a %d comme binome mais %d a %d comme binome\n",
				       sciper1,sciper2,sciper2,sciper3);
				return -1;
			   }
		    }
		}
	}  
    return 0;
} 

// ----------------------------------------------
void get_students_by_type(const Database db, StudentKind type, QueryResult result_out)
{
	//Initialiser le tableau result_out
	for(int i=0; i<QUERY_MAX_SIZE; i++){
		result_out[i] = NULL;
	}
	//Parcours de la base de donnee db
	int i = 0; 
	int j = 0;
	do{
		if(db[i].sciper != 0 && db[i].type == type){
			result_out[j] = &db[i];
			j++;
		}
		i++;
	}while(i<DB_MAX_SIZE && j<QUERY_MAX_SIZE);
}

// ----------------------------------------------
double grade_average(const Student* stud)
{
    if (stud == NULL) return 0.0;
    return 0.1 * stud->grade_hw + 0.4 * stud->grade_sn + 0.5 * stud->grade_exam;
}

// ----------------------------------------------
double team_diff(const Student* stud)
{
    return fabs(grade_average(stud) - grade_average(stud->teammate));
}

// ----------------------------------------------
#define student_to_index(tab, student) (size_t)((student) - (tab))

// ----------------------------------------------
void get_least_homegenous_teams(const Database db, QueryResult result_out)
{
	//Initialiser le tableau result_out
	memset(result_out, 0, QUERY_MAX_SIZE);
	//Nombre des equipes parcourues
	int numberOfTeams = 0;
	//Total de difference maximale pour entrer dans le tableau result_out
    double minDiff = 6;
	//Meilleur binome dans le tableau result_out
	const Student* bestInQuery = NULL;
	//Parcours de la base de donnée db
	for(int i=0; i<DB_MAX_SIZE; i++){
		if(db[i].teammate != NULL && (student_to_index(db,&db[i]) < student_to_index(db,db[i].teammate))){
			numberOfTeams++;
			//Pointeur vers le pire binome de l'equipe et calcul de la difference des notes
			const Student* worstInTeam = NULL;
			if(grade_average(&db[i]) < grade_average(db[i].teammate)){
                  worstInTeam = &db[i];
			}else{
				  worstInTeam = db[i].teammate;
			}
			double diff = team_diff(worstInTeam);
			//Cas où le tableau result_out n'est pas encore rempli
			if(numberOfTeams <= QUERY_MAX_SIZE){
				result_out[numberOfTeams-1] = worstInTeam;
				//On met minDiff à jour 
				if(diff < minDiff){
                    minDiff = diff;
                    bestInQuery = worstInTeam;
				}
			}
			//Cas où le tableau result_out est rempli et on a une equipe à placer (pire que une de celles deja presentes)
			else if(diff > minDiff){
				//On met à jour le tableau
				for(int j=0; j<QUERY_MAX_SIZE; j++){
					if((*result_out[j]).sciper == (*bestInQuery).sciper){
						result_out[j] = worstInTeam;
					}
				}
				//On met à jour minDiff et bestInQuery
				minDiff = 6;
				for(int j=0; j<QUERY_MAX_SIZE; j++){
					double diffTeam = team_diff(result_out[j]);
					if(diffTeam < minDiff){
						minDiff = diffTeam;
                        bestInQuery = result_out[j];
					}
				}
			}

		}
	}	
}

// ----------------------------------------------
//  ___   _____
// |_ _| / / _ \
//  | | / / (_) |
// |___/_/ \___/

//Check si le fichier réussi à lire une certaine valeur
int checkError(FILE* fichier, const char* filename, int test){
	if(test != 1){
		fprintf(stderr, "Erreur: Impossible de lire une certaine valeur dans le fichier %s: ",filename);
		return -1;
	}
	return 0;
}
int load_database(Database db_out, const char* filename)
{
	//Initialiser la base de donnee db_out avec des etudiants au sciper = 0
	memset(db_out, 0, DB_MAX_SIZE);
	//Initialiser le tableau des scipers des binomes
	SCIPER all_teammate_scipers[DB_MAX_SIZE];
	memset(all_teammate_scipers, 0, DB_MAX_SIZE);
	//Ouverture du fichier
    FILE* fichier = fopen(filename, "r");
    if(fichier == NULL){
		fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s pour lecture: ", filename);
		return -1;
	}
	//Lecture du fichier et lancement d'erreur en cas d'incoherence
	int i = 0;
	SCIPER student_sciper = 0;
	int test = fscanf(fichier, "%" SCNu32, &student_sciper);
	while(!feof(fichier)){

		if(checkError(fichier, filename ,test) == -1) return -1;
		Student student = {0,0,0,0,0,NULL};
		student.sciper = student_sciper;
		test = fscanf(fichier, "%lf", &student.grade_sn);
		if(checkError(fichier, filename ,test) == -1) return -1;
		test = fscanf(fichier, "%lf", &student.grade_hw);
		if(checkError(fichier, filename ,test) == -1) return -1;
		test = fscanf(fichier, "%lf", &student.grade_exam);
		if(checkError(fichier, filename ,test) == -1) return -1;
		int type = 0;
		test = fscanf(fichier, "%d", &type);
		if(checkError(fichier, filename ,test) == -1) return -1;
		if(type != Exchange && type != Bachelor && type != Master){
			fprintf(stderr, "Erreur: Type inconnu dans le fichier %s: ", filename);
			return -1;
		}
		student.type = type;
		SCIPER sciper_teammate = 0;
		test = fscanf(fichier, "%" SCNu32, &sciper_teammate);
		if(checkError(fichier, filename ,test) == -1) return -1;
		all_teammate_scipers[i] = sciper_teammate;
		db_out[i] = student;
		i++;
		test = fscanf(fichier, "%" SCNu32, &student_sciper);
	};
    //Pointer les binomes
    for(int i=0; i<DB_MAX_SIZE; i++){
		db_out[i].teammate = get_student_by_sciper(db_out, all_teammate_scipers[i]);
	}
    //Fermer le fichier
    fclose(fichier);
    //Check des pointers (pairs)
    return check_teammates_consistency(db_out);
}

// ----------------------------------------------
void fprintf_student_kind(FILE* restrict stream, StudentKind sk)
{
    switch (sk) {
    case Bachelor: fputs("bachelor", stream); break;
    case Master:   fputs("master  ", stream); break;
    case Exchange: fputs("exchange", stream); break;
    default:       fputs("unknown ", stream); break;
    }
}

// ----------------------------------------------
void write_student(const Student* student, FILE* fp)
{
    fprintf(fp, "%07d - %.2f, %.2f, %.2f - ",
            student->sciper, student->grade_sn, student->grade_hw,
            student->grade_exam);
    fprintf_student_kind(fp, student->type);
    if (student->teammate != NULL) {
        fprintf(fp, " - %06" PRIu32, student->teammate->sciper);
    } else {
        fprintf(fp, " - none");
    }
    fprintf(fp, "\n");
}

// ----------------------------------------------
int write_query_results(QueryResult result, const char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file %s for writting: ", filename);
        perror(NULL); // optionnel
        return -1;
    }
    for (size_t i = 0; i < QUERY_MAX_SIZE && result[i] != NULL; ++i) {
        write_student(result[i], fp);
    }
    fclose(fp);
    printf("Query results saved to: %s\n", filename);
    return 0;
}

// ----------------------------------------------
//  __  __      _
// |  \/  |__ _(_)_ _
// | |\/| / _` | | ' \
// |_|  |_\__,_|_|_||_|

int main(int argc, char** argv)
{
    const char* input_filename = "data.txt"; // default input filename
    if (argc >= 2) {
        input_filename = argv[1];
    }

    Database db;
    memset(db, 0, sizeof(db));
    int success = load_database(db, input_filename);
    if (success != 0) {
        fputs("Could not load database.\n", stderr);
        return EXIT_FAILURE;
    }
    // Print contents of database
    puts("+ ----------------- +\n"
         "| Students database |\n"
         "+ ----------------- +");
    const size_t end = db_entry_count(db);
    for (size_t i = 0; i < end; ++i) {
        write_student(&db[i], stdout);
    }

    // Extract students of each kind
    QueryResult res;
    memset(res, 0, sizeof(res));
    char filename[] = "res_type_00.txt";
    const size_t filename_mem_size = strlen(filename) + 1;
    for (StudentKind sk = Bachelor; sk < StudentKindCount; ++sk) {
        get_students_by_type(db, sk, res);
        snprintf(filename, filename_mem_size, "res_type_%02d.txt", sk);
        write_query_results(res, filename);
    }

   //Extract least homogeneous teams
     get_least_homegenous_teams(db, res);
     write_query_results(res, "bad_teams.txt");

    return EXIT_SUCCESS;
}
