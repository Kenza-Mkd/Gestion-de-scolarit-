#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#pragma warning(disable:4996)

enum {
	ETUDIANTS_MAX = 100, // Nombre maximum d'étudiants gérés
	NB_UE = 6, // Nombre d'UE par semestre 
	NB_SEMESTRES = 6, // Nombre total de semestres
	MAX_NOM = 31, // 30 caractères max pour nom/prénom + 1 pour '\0'
	CHARA_MAX = 31, // Taille max pour la chaîne de commande
	NB_ANNEE = 3  // NB d'année
};

// Statuts des etudiants
typedef enum { EN_COURS, DEFAILLANT, DEMISSION, DIPLOME, AJOURNE } Statut;
enum Statutnote { INC, ADM, AJ, AJB, ADC, ADS };

// Structure NOTE
typedef struct {
	float note;
	int statutnote;
}Note;

const float NOTE_MIN = 0.f, NOTE_MAX = 20.f;


// Structure ETUDIANT
typedef struct {
	char prenom[31];
	char nom[31];
	Note notes[NB_SEMESTRES][NB_UE];
	int semestre_courant; //Le semestre en cours de l'étudiant
	Statut statut;
}Etudiant;

// Fonction permettant de retourner la moyenne de deux notes pour le RCUE
float Moyenne(float note1, float note2) {
	return (note1 + note2) / 2;
}

/**
 * Convertit un énum 'Statut' en chaîne de caractères pour l'affichage.
 * Requis par les commandes CURSUS et ETUDIANTS
 */
char* affichestatut(Statut statut) {
	switch (statut) {
	case DEMISSION:
		return "demission";
	case DEFAILLANT:
		return "defaillance";
	case DIPLOME:
		return "diplome";
	case AJOURNE:
		return "ajourne";
	default:
		return "en cours";
	}
}

// Fonction EXIT permettant de quitter le programme
void sortie() {
	exit(0);
}

// FONCTION INSCRIRE : permet d'inscrire un étudiant dans la promotion
void inscrire(Etudiant promo[], int* etudiantnb) {
	Etudiant etu;
	if (*etudiantnb >= ETUDIANTS_MAX) {
		printf("Nombre maximum d'etudiants atteint\n");
		return;
	}
	scanf("%30s %30s", etu.prenom, etu.nom); // Prénom et nom de l'étudiant, le 30 correspond à MAX_NOM - 1 pour le caractère de fin de chaîne '\0'

	// Vérification des doublons (même nom ET même prénom)
	int existedeja = 0;
	for (int i = 0; i < *etudiantnb; ++i) {
		if (strcmp(promo[i].nom, etu.nom) == 0 && strcmp(promo[i].prenom, etu.prenom) == 0) {
			existedeja = 1;
			printf("Nom incorrect\n");
			return;
		}
	}

	// Si l'étudiant n'existe pas
	if (!existedeja) {
		// Initialisation des notes à -1 (pour signifier "inconnue" : * (*))
		for (int ue = 0; ue < NB_UE; ue++)
			for (int sem = 0; sem < NB_SEMESTRES; sem++)
				etu.notes[sem][ue].note = -1.f;

		etu.statut = EN_COURS; // Statut par défaut
		etu.semestre_courant = 1; // Semestre par défaut
		promo[*etudiantnb] = etu; // Ajout au tableau
		(*etudiantnb)++; // Incrémentation du compteur
		printf("Inscription enregistree (%d)\n", *etudiantnb);
	}
	else {
		printf("Nom incorrect\n");
	}
}

// FONCTION CURSUS : affiche le cursus complet d'un étudiant
void cursus(Etudiant promo[], int etudiantnb, int id) {
	// Vérification de l'identifiant
	if (id < 1 || id > etudiantnb) {
		printf("Identifiant incorrect\n");
		return;
	}
	Etudiant* etu = &promo[id - 1]; // Pointeur vers l'étudiant (id-1 pour l'index)
	printf("%d %s %s\n", id, etu->prenom, etu->nom);

	// Détermine jusqu'à quel semestre afficher les détails
	int semestre_max = etu->semestre_courant;

	// Affiche chaque semestre passé et le semestre courant
	for (int sem = 1; sem <= semestre_max; sem++) {
		if (sem == 1)
			printf("S%d -", sem);
		else
			printf("\nS%d -", sem);

		// Boucle sur les 6 UEs du semestre
		for (int u = 0; u < NB_UE; u++) {
			float note = etu->notes[sem - 1][u].note;
			if (note < 0.f) {
				printf(" * (*) -");
			}
			else {
				// Affichage du statut de la note
				printf(" %.1f", floorf(note * 10) / 10);
				if (etu->notes[sem - 1][u].statutnote == ADM) {
					printf(" (ADM) -");
				}
				else if (etu->notes[sem - 1][u].statutnote == AJ) {
					printf(" (AJ) -");
				}
				else if (etu->notes[sem - 1][u].statutnote == ADC) {
					printf(" (ADC) -");
				}
				else if (etu->notes[sem - 1][u].statutnote == ADS) {
					printf(" (ADS) -");
				}
			}
		}
		// Affiche chaque semestre passé et le semestre courant
		if (sem % 2 == 0 && (sem < semestre_max || etu->statut == DIPLOME || etu->statut == AJOURNE)) {
			printf("\nB%d -", sem / 2);
			for (int ue = 0; ue < NB_UE; ue++) {
				float moyenne = Moyenne(etu->notes[sem - 1][ue].note, etu->notes[sem - 2][ue].note);
				printf(" %.1f", floorf(moyenne * 10) / 10);

				if (moyenne >= 10.f) {
					printf(" (ADM) -");
				}
				else if (moyenne < 8.f) {
					printf(" (AJB) -");
				}
				else if (moyenne >= 8 && moyenne < 10) {
					if (etu->notes[sem - 1][ue].statutnote == ADS || etu->notes[sem - 2][ue].statutnote == ADS) {
						printf(" (ADS) -");
					}
					else {
						printf(" (AJ) -");
					}
				}
			}
		}
	}
	printf(" %s\n", affichestatut(etu->statut));

}

// FONCTION NOTE : Enregistre une note pour un étudiant dans une UE donnée
void note(Etudiant promo[], int etudiantnb, int id, int ue, float note) {
	// Vérification de l'identifiant
	if (id < 1 || id > etudiantnb) {
		printf("Identifiant incorrect\n");
		return;
	}
	Etudiant* etu = &promo[id - 1];
	// Vérification du statut de l'étudian
	if (etu->statut != EN_COURS) {
		printf("Etudiant hors formation\n");
		return;
	}
	// Vérification du numéro d'UE
	if (ue < 1 || ue > NB_UE) {
		printf("UE incorrecte\n");
		return;
	}
	// Vérification de la valeur de la note
	if (note < NOTE_MIN || note > NOTE_MAX) {
		printf("Note incorrecte\n");
		return;
	}
	// Enregistrement de la note dans le semestre courant de l'étudiant
	int semestre_index = etu->semestre_courant - 1;
	etu->notes[semestre_index][ue - 1].note = note;
	// Attribution du statut initial de la note (ADM ou AJ)
	if (note >= 10) {
		etu->notes[semestre_index][ue - 1].statutnote = ADM;
	}
	else {
		etu->notes[semestre_index][ue - 1].statutnote = AJ;
	}
	printf("Note enregistree\n");
}

// FONCTION DEMISSION : Enregistre la démission d'un étudiant
void demission(Etudiant promo[], int etudiantnb, int id) {
	if (id < 1 || id > etudiantnb) {
		printf("Identifiant incorrect\n");
		return;
	}
	Etudiant* etu = &promo[id - 1];
	// Seulement si l'étudiant est "en cours"
	if (etu->statut == EN_COURS) {
		etu->statut = DEMISSION;
		printf("Demission enregistree\n");
	}
	else {
		printf("Etudiant hors formation\n");
	}
}

// FONCTION DEFAILLANCE : Enregistre la défaillance d'un étudiant
void defaillance(Etudiant promo[], int etudiantnb, int id) {
	if (id < 1 || id > etudiantnb) {
		printf("Identifiant incorrect\n");
		return;
	}
	Etudiant* etu = &promo[id - 1];
	// Seulement si l'étudiant est "en cours"
	if (etu->statut == EN_COURS) {
		etu->statut = DEFAILLANT;
		printf("Defaillance enregistree\n");
	}
	else {
		printf("Etudiant hors formation\n");
	}
}

//FONCTION JURY : Traite le jury pour un semestre donné pour tous les étudiants	concernés
void jury(Etudiant promo[], int etudiantnb, int* pSemestre) {
	int semestre = *pSemestre;
	int nb_etu_formee = 0; // Compteur pour le message final

	// Vérification de la validité du semestre
	if (semestre < 1 || semestre > NB_SEMESTRES) {
		printf("Semestre incorrect\n");
		return;
	}

	// 1. Vérification des notes manquantes
	for (int i = 0; i < etudiantnb; i++) {
		Etudiant* etu = &promo[i];
		// On ne traite que les étudiants "en cours" ET inscrits au semestre du jury
		if (!(etu->statut == EN_COURS && etu->semestre_courant == semestre))
			continue;

		// Vérifier si toutes les notes de ce semestre sont saisies
		for (int ue = 0; ue < NB_UE; ue++) {
			if (etu->notes[semestre - 1][ue].note < 0.f) {
				printf("Des notes sont manquantes\n");
				return;
			}
		}
		// Cas 1: Jury de semestre IMPAIR (S1, S3, S5)
		if (semestre % 2 == 1) {
			// Passage automatique au semestre suivant
			etu->semestre_courant++;
			nb_etu_formee++;
			continue;
		}

		// Cas 2: Jury de semestre PAIR (S2, S4, S6)
		// C'est ici qu'on calcule les RCUE et qu'on applique les règles de passage/ajournement
		int ue_validees = 0;        // RCUE >= 10
		int ue_ajournees = 0;       // RCUE < 8
		int ue_validees_prev = 0;   // Compteur d'UE validées l'année N-1

		// Calcul des RCUE et application des compensations (ADC) et dettes (ADS)
		for (int ue = 0; ue < NB_UE; ue++) {
			// Notes de l'année en cours
			float n1 = etu->notes[semestre - 2][ue].note; // semestre précédent
			float n2 = etu->notes[semestre - 1][ue].note; // semestre courant
			float moy = Moyenne(n1, n2);

			// Règle 1: Compensation (ADC)
			if (moy >= 10.f) {
				// marquer ADC si nécessaire (conversion d'un AJ des semestres en ADC)
				if (etu->notes[semestre - 2][ue].statutnote == AJ)
					etu->notes[semestre - 2][ue].statutnote = ADC;
				if (etu->notes[semestre - 1][ue].statutnote == AJ)
					etu->notes[semestre - 1][ue].statutnote = ADC;

				ue_validees++;
			}
			else if (moy < 8.f) {
				ue_ajournees++;
			}

			// Règle 2: Validation N-1 (ADS) - S'applique pour jury S4 et S6
			if (semestre == 4 || semestre == 6) {
				// Notes de l'année N-1 (ex: S1 et S2 pour le jury S4)
				float prev_n1 = etu->notes[semestre - 4][ue].note; // sem-4
				float prev_n2 = etu->notes[semestre - 3][ue].note; // sem-3
				float prev_moy = Moyenne(prev_n1, prev_n2); // RCUE de l'année N-1

				if (prev_moy >= 10.f) {
					ue_validees_prev++; // Déjà validée
				}
				// Si RCUE N-1 était AJ (8-10) MAIS RCUE N est >= 10 (moy)
				else if (prev_moy >= 8.f && prev_moy < 10.f && moy >= 10.f) {
					// convertir AJ -> ADS 
					if (etu->notes[semestre - 4][ue].statutnote == AJ)
						etu->notes[semestre - 4][ue].statutnote = ADS;
					if (etu->notes[semestre - 3][ue].statutnote == AJ)
						etu->notes[semestre - 3][ue].statutnote = ADS;
					ue_validees_prev++;
				}
			}
		}
		// Règle d'ajournement prioritaire : RCUE < 8
		if (ue_ajournees > 0) {
			etu->statut = AJOURNE;
		}
		else {
			// Pas de RCUE < 8, on vérifie les règles de passage
			// Jury S2 (Passage en B2)
			if (semestre == 2) {
				if (ue_validees >= 4) {
					etu->semestre_courant++;
				}
				else {
					etu->statut = AJOURNE;
				}
			}
			// Jury S4 (Passage en B3)
			else if (semestre == 4) {
				// pour B2 on exige : nb_UE_valide_precedente == NB_UE et au moins 4 UE validées dans le bloc courant
				if (ue_validees >= 4 && ue_validees_prev == NB_UE) {
					etu->semestre_courant++;
				}
				else {
					etu->statut = AJOURNE;
				}
			}
			// Jury S6 (Diplomation)
			else if (semestre == 6) {
				// diplôme si toutes les UE validées dans B3 et B2 (selon règles)
				if (ue_validees == NB_UE && ue_validees_prev == NB_UE) {
					etu->statut = DIPLOME;
				}
				else {
					etu->statut = AJOURNE;
				}
			}
		}
		nb_etu_formee++; // Compteur d'étudiants traités
	}
	// Affichage final
	printf("Semestre termine pour %d etudiant(s)\n", nb_etu_formee);
}

// FONCTION ETUDIANTS : Affiche la liste des étudiants inscrits
void etudiants(Etudiant promo[], int etudiantnb) {
	for (int i = 0; i < etudiantnb; ++i) {
		Etudiant* etu = &promo[i];
		printf("%d - %s %s - S%d - %s\n",
			i + 1,                 // L'identifiant 
			etu->prenom,
			etu->nom,
			etu->semestre_courant,
			affichestatut(etu->statut)); // Conversion enum -> string
	}
}

// FONCTION BILAN : Affiche un bilan statistique pour une année donnée (B1, B2 ou B3)
void bilan(Etudiant promo[], int etudiantnb, int annee) {
	int demissionnaire = 0;
	int defaillance = 0;
	int encour = 0;
	int ajournee = 0;
	int passee = 0;

	// Vérification de l'année
	if (annee < 1 || annee > NB_ANNEE) {
		printf("Annee incorrecte\n");
		return;
	}

	// Déterminer les semestres concernés par l'année
	int sem_debut_annee = (annee - 1) * 2 + 1; // Annee 1 -> S1, Annee 2 -> S3, ...
	int sem_fin_annee = annee * 2;           // Annee 1 -> S2, Annee 2 -> S4, ...

	// Parcours de tous les étudiants pour les catégoriser
	for (int i = 0; i < etudiantnb; i++) {
		Etudiant* etu = &promo[i];

		// Ignorer les étudiants n'ayant pas encore atteint cette année
		if (etu->semestre_courant < sem_debut_annee) {
			continue;
		}
		// Catégorisation selon le statut et le semestre courant
		if (etu->statut == DIPLOME || etu->semestre_courant > sem_fin_annee) {
			passee++;
		}
		else if (etu->statut == AJOURNE && etu->semestre_courant == sem_fin_annee) {
			ajournee++;
		}
		else if (etu->statut == EN_COURS && (etu->semestre_courant == sem_debut_annee || etu->semestre_courant == sem_fin_annee)) {
			encour++;
		}
		else if (etu->statut == DEMISSION && (etu->semestre_courant == sem_debut_annee || etu->semestre_courant == sem_fin_annee)) {
			demissionnaire++;
		}
		else if (etu->statut == DEFAILLANT && (etu->semestre_courant == sem_debut_annee || etu->semestre_courant == sem_fin_annee)) {
			defaillance++;
		}
	}
	// Affichage des résultats
	printf("%d demission(s)\n", demissionnaire);
	printf("%d defaillance(s)\n", defaillance);
	printf("%d en cours\n", encour);
	printf("%d ajourne(s)\n", ajournee);
	printf("%d passe(s)\n", passee);
}

int main() {
	int etudiantnb = 0; // Compteur global du nombre d'étudiants inscrits
	Etudiant promo[ETUDIANTS_MAX]; // Tableau principal contenant tous les étudiants
	char commande[CHARA_MAX]; // Buffer pour lire la commande
	int semestre; // Variable pour la commande JURY
	int annee; // Variable pour la commande BILAN

	// Boucle principale
	do {
		scanf("%s", commande);

		if (strcmp(commande, "INSCRIRE") == 0) {
			inscrire(promo, &etudiantnb);
		}
		else if (strcmp(commande, "CURSUS") == 0) {
			int id;
			scanf("%d", &id);
			cursus(promo, etudiantnb, id);
		}
		else if (strcmp(commande, "NOTE") == 0) {
			int id, ue;
			float notes;
			scanf("%d %d %f", &id, &ue, &notes);
			note(promo, etudiantnb, id, ue, notes);
		}
		else if (strcmp(commande, "DEMISSION") == 0) {
			int id;
			scanf("%d", &id);
			demission(promo, etudiantnb, id);
		}
		else if (strcmp(commande, "DEFAILLANCE") == 0) {
			int id;
			scanf("%d", &id);
			defaillance(promo, etudiantnb, id);
		}
		else if (strcmp(commande, "JURY") == 0) {
			scanf("%d", &semestre);
			jury(promo, etudiantnb, &semestre);
		}
		else if (strcmp(commande, "ETUDIANTS") == 0) {
			etudiants(promo, etudiantnb);
		}
		else if (strcmp(commande, "BILAN") == 0) {
			scanf("%d", &annee);
			bilan(promo, etudiantnb, annee);
		}
		else if (strcmp(commande, "EXIT") == 0) {
			sortie();
		}
		else {
			printf("Commande inconnue\n");
		}
	} while (1);
}
