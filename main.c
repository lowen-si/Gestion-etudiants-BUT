// Ajout des bibliotheques
#include <stdio.h>
#include <string.h>
#include <math.h>

// Directives de compilation
#pragma warning(disable:4996)

//on definit les constantes numeriques
enum { MAX_ETU = 100, MAX_CARACTERES = 31, MAX_SEMESTRE = 6, NBR_UE = 6, TAILLE_CODE_UE = 4, VIDE = -1 };
const float NOTE_MIN = 0.f, NOTE_AJ = 8.f, NOTE_MOY = 10.f, NOTE_MAX = 20.f, NOTE_INCONNUE = -1.f;
enum { ANNEE1 = 1, ANNEE2, ANNEE3 };

//on defint un type enumere booleen
typedef enum{ FALSE, TRUE } Bool;

// type enumere pour le numero du Semestre
typedef enum { S1 = 1, S2, S3, S4, S5, S6 } Num_Semestre;

//on definit l'etat du semestre qui peut etre fini, en cours ou non commence
typedef struct {
    char code[TAILLE_CODE_UE];
    float note;
} Ue;

//on definit l'etat du semestre
typedef enum { FINI, EN_COURS, NON_COMMENCE, DEMISSION, DEFAILLANCE, AJOURNE, DIPLOME } Etat_semestre;

// on definit un type Semestre qui a la note et le semestre
typedef struct {
    Num_Semestre num_semestre;
    Ue ue[NBR_UE];
    Etat_semestre etat_semestre;
} Semestre;

//on definit un nouveau type Etudiant qui aura l'identifiant, le prenom et le nom de l'etudiant
typedef struct {
    int id;
    char Prenom[MAX_CARACTERES];
    char Nom[MAX_CARACTERES];
    Semestre cursus[MAX_SEMESTRE];
} Etudiants;

// Promotion qui contient le Nombre d'etudiant de la promo et la liste de tout les etudiants
typedef struct {
    int nbEtudiant;
    Etudiants liste_etu[MAX_ETU];
} Promotion;

// definition des constantes chaines de caracteres correspondantes aux commandes
const char commande_exit[] = "EXIT", commande_inscrire[] = "INSCRIRE", commande_cursus[] = "CURSUS",
commande_note[] = "NOTE", commande_etudiants[] = "ETUDIANTS", commande_demission[] = "DEMISSION",
commande_defaillance[] = "DEFAILLANCE", commande_jury[] = "JURY", commande_bilan[] = "BILAN";

// retourne le numero (1..6) du semestre en cours pour l'etudiant id, ou -1 si aucun
int semestre_en_cours(const Promotion* promotion, const int id) {
    if (id <= 0 || id > MAX_ETU) return -1;
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[id - 1].cursus[i].etat_semestre == EN_COURS)
            return i + 1;
    }
    return -1;
}

//fonction qui initialise le cursus de l'etudiant. Prends en parametre l'identifiant de l'etudiant
void initialiser_cursus(Promotion* promotion, const int id, int semestre) {
    if (id <= 0 || id > MAX_ETU || semestre < 1 || semestre > MAX_SEMESTRE)
        return;
    int indice = id - 1;
    promotion->liste_etu[indice].cursus[semestre - 1].num_semestre = semestre;
    promotion->liste_etu[indice].cursus[semestre - 1].etat_semestre = EN_COURS;
    if (semestre == S1) {
        for (int i = 1; i < MAX_SEMESTRE; ++i)
            promotion->liste_etu[indice].cursus[i].etat_semestre = NON_COMMENCE;
    }
    for (int i = 0; i < NBR_UE; ++i) {
        promotion->liste_etu[indice].cursus[semestre - 1].ue[i].note = NOTE_INCONNUE;
        strcpy(promotion->liste_etu[indice].cursus[semestre - 1].ue[i].code, "*");
    }
}

// fonction qui renvoie 1 si l'etudiant est deja enregistree et 0 s'il ne l'est pas
int estEnregistre(const Promotion* promotion, const int id) {
    if (id <= 0 || id > MAX_ETU) return 0;
    if (id <= promotion->nbEtudiant)
        return 1;
    return 0;
}

// fonction qui permet d'inscrire un etudiant dans la liste_etu si il n'est pas deja inscrit
void inscription(Promotion* promotion, const Etudiants etu) {
    int continuer = TRUE;
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (strcmp(promotion->liste_etu[i].Prenom, etu.Prenom) == 0 &&
            strcmp(promotion->liste_etu[i].Nom, etu.Nom) == 0) {
            printf("Nom incorrect\n");
            continuer = FALSE;
        }
    }
    if (continuer == TRUE) {
        if (promotion->nbEtudiant >= MAX_ETU) {
            printf("Inscription impossible\n");
            return;
        }
        ++promotion->nbEtudiant;
        strcpy(promotion->liste_etu[promotion->nbEtudiant - 1].Prenom, etu.Prenom);
        strcpy(promotion->liste_etu[promotion->nbEtudiant - 1].Nom, etu.Nom);
        promotion->liste_etu[promotion->nbEtudiant - 1].id = promotion->nbEtudiant;
        printf("Inscription enregistree (%d)\n", promotion->nbEtudiant);
        initialiser_cursus(promotion, promotion->nbEtudiant, S1);
    }
}

//fonction qui recupere les noms / prenoms saisies lors de l'inscription
void saisie_nom(Promotion* promotion) {
    char prenom_saisie[MAX_CARACTERES], nom_saisie[MAX_CARACTERES];
    scanf("%s", prenom_saisie);
    scanf("%s", nom_saisie);
    Etudiants etudiant;
    etudiant.id = VIDE;
    strcpy(etudiant.Prenom, prenom_saisie);
    strcpy(etudiant.Nom, nom_saisie);
    inscription(promotion, etudiant);
}

// fonction qui permet a l'utilsateur d'entrer une note
void saisie_note(Promotion* promotion) {
    int id_saisie, code_saisie;
    float note_saisie;
    scanf("%d", &id_saisie);
    scanf("%d", &code_saisie);
    scanf("%f", &note_saisie);
    int semestre = semestre_en_cours(promotion, id_saisie);
    if (code_saisie < 1 || code_saisie > NBR_UE)
        printf("UE incorrecte\n");
    else if (estEnregistre(promotion, id_saisie) == FALSE)
        printf("Identifiant incorrect\n");
    else if (semestre == -1 || promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].etat_semestre != EN_COURS)
        printf("Etudiant hors formation\n");
    else if (note_saisie < NOTE_MIN || note_saisie > NOTE_MAX)
        printf("Note incorrecte\n");
    else {
        promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].ue[code_saisie - 1].note = note_saisie;
        if (note_saisie < NOTE_MOY)
            strcpy(promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].ue[code_saisie - 1].code, "AJ");
        else strcpy(promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].ue[code_saisie - 1].code, "ADM");
        printf("Note enregistree\n");
    }
}

// fonction qui affiche l'etat des semestres
void affiche_etat_semestre(const Etat_semestre et) {
    if (et == EN_COURS)
        printf("- en cours\n");
    else if (et == DEMISSION)
        printf("- demission\n");
    else if (et == DEFAILLANCE)
        printf("- defaillance\n");
    else if (et == AJOURNE)
        printf("- ajourne\n");
    else if (et == DIPLOME)
        printf("- diplome\n");
    else
        printf("-\n");
}

// fonction qui affiche le code en fonction de la moyenne rcue
void affiche_code_rcue(const Promotion* promotion, float note, int id_saisie, int semestre_index, int num_ue) {
    if (note < NOTE_AJ)
        printf("AJB");
    else if (note >= NOTE_AJ && note < NOTE_MOY && (strcmp(promotion->liste_etu[id_saisie - 1].cursus[semestre_index - 1].
        ue[num_ue].code, "ADS") == 0 || (semestre_index < MAX_SEMESTRE - 1 && strcmp(promotion->
            liste_etu[id_saisie - 1].cursus[semestre_index].ue[num_ue].code, "ADS") == 0)))
        printf("ADS");
    else if (note >= NOTE_AJ && note < NOTE_MOY)
        printf("AJ");
    else printf("ADM");
}

// fonction qui calcule la note moyenne rcue de l'indice du semestre (et de celui d'avant) et de l'ue pris en parametre
float calcul_rcue(const Promotion* promotion, int id_etu, const int indice_semestre, int num_ue) {
    if (id_etu <= 0 || id_etu > MAX_ETU) return NOTE_INCONNUE;
    if (indice_semestre <= 0 || indice_semestre >= MAX_SEMESTRE) return NOTE_INCONNUE;
    float note_actuelle = promotion->liste_etu[id_etu - 1].cursus[indice_semestre].ue[num_ue].note;
    float note_precedente = promotion->liste_etu[id_etu - 1].cursus[indice_semestre - 1].ue[num_ue].note;
    if (note_actuelle < NOTE_MIN || note_precedente < NOTE_MIN) return NOTE_INCONNUE;
    return floorf(((note_actuelle + note_precedente) / 2.0f) * 10.f) / 10.f;
}

// fonction qui renvoie 1 si l'etudiant doit etre ajourne et renvoie 0 si l'etudiant ne doit pas etre ajourne
// ou l'est deja
int test_ajourne_etu(const Promotion* promotion, int id_saisie, int semestre_index) {
    if (id_saisie <= 0 || id_saisie > MAX_ETU) return 0;
    Etat_semestre etat = promotion->liste_etu[id_saisie - 1].cursus[semestre_index].etat_semestre;
    if (etat == DEMISSION || etat == DEFAILLANCE || etat == NON_COMMENCE || etat == AJOURNE) return 0;
    int nbr_aj = 0;
    for (int i = 0; i < NBR_UE; ++i) {
        float rcue = calcul_rcue(promotion, id_saisie, semestre_index, i);
        if (rcue < NOTE_MIN)
            return 0;  // RCUE invalide : on n'ajourne pas
        if (rcue < NOTE_AJ)
            return 1;
        if (rcue < NOTE_MOY && rcue >= NOTE_AJ)
            ++nbr_aj;
    }
    if (nbr_aj >= NBR_UE / 2)
        return 1;
    return 0;
}

//fonction qui ajourne l'etudiant a l'indice du semestre pris en parametres
void ajourne_etu(Promotion* promotion, int id_etu, int indice_semestre) {
    promotion->liste_etu[id_etu - 1].cursus[indice_semestre].etat_semestre = AJOURNE;
    for (int i = indice_semestre + 1; i < NBR_UE; ++i) {
        promotion->liste_etu[id_etu - 1].cursus[i].etat_semestre = NON_COMMENCE;
    }
}

//fonction qui réalise la transformation en ADC d'un etudiant au semestre pris en parametre
void adc(Promotion* promotion, int semestre, int indice_etu) {
    int idice_sem = semestre - 1;
    for (int i = 0; i < NBR_UE; ++i) {
        float rcue = calcul_rcue(promotion, indice_etu + 1, idice_sem, i);
        if (rcue < NOTE_MIN) continue; // pas de RCUE valide
        // si RCUE >= 10 : compense les UE de 8 a 10 dans les deux semestres
        if (rcue >= NOTE_MOY) {
            // semestre precedent (impair)
            if (promotion->liste_etu[indice_etu].cursus[idice_sem - 1].ue[i].note < NOTE_MOY && promotion->liste_etu[indice_etu].cursus[idice_sem - 1].etat_semestre != DEFAILLANCE) {
                strcpy(promotion->liste_etu[indice_etu].cursus[idice_sem - 1].ue[i].code, "ADC");
            }
            // semestre actuel (pair)
            if (promotion->liste_etu[indice_etu].cursus[idice_sem].ue[i].note < NOTE_MOY && promotion->liste_etu[indice_etu].cursus[idice_sem].etat_semestre != DEFAILLANCE) {
                strcpy(promotion->liste_etu[indice_etu].cursus[idice_sem].ue[i].code, "ADC");
            }
        }
    }
}

//test de la validation par application de la regle de compensation a la fin des semestres pairs
void test_adc(Promotion* promotion, int semestre) {
    if (semestre < S2 || semestre > MAX_SEMESTRE)
        return; // rien  faire pour S1
    for (int k = 0; k < promotion->nbEtudiant; ++k) {
        if (promotion->liste_etu[k].id == VIDE) continue;
        //  Ignorer les etudiants qui ont demissionnes, defaillants ou ajournes
        Etat_semestre etat_pair = promotion->liste_etu[k].cursus[semestre - 1].etat_semestre;
        Etat_semestre etat_impair = promotion->liste_etu[k].cursus[semestre - 2].etat_semestre;
        if (etat_pair == DEFAILLANCE || etat_pair == DEMISSION ||
            etat_impair == DEFAILLANCE || etat_impair == DEMISSION)
            continue;
        adc(promotion, semestre, k);
    }
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id != VIDE)
            if (test_ajourne_etu(promotion, i + 1, semestre - 1))
                ajourne_etu(promotion, i + 1, semestre - 1);
    }
}

void affiche_etat_annee3(const Promotion* promotion, int id_etu) {
    int indice_sem = S6 - 1;
    for (int i = 0; i < NBR_UE; ++i) {
        float rcue = calcul_rcue(promotion, id_etu, indice_sem, i);
        if (rcue < NOTE_MOY) {
            printf(" ajourne");
            return;
        }
    }
    printf(" diplome");
}

// fonction qui affiche l'annee avec les moyennes rcue
void affiche_annee(Promotion* promotion, int id_etu, const int* indice_semestre) {
    int indice_sem = *indice_semestre;
    printf("B%d ", indice_sem == S6 - 1 ? 3 : indice_sem == S4 - 1 ? 2 : 1);
    for (int k = 0; k < NBR_UE; ++k) {
        float rcue = calcul_rcue(promotion, id_etu, indice_sem, k);
        if (rcue < NOTE_MIN) {
            printf("- * (*) ");
            continue;
        }
        if (indice_sem > 1) {
            for (int i = indice_sem - 1; i <= indice_sem; ++i)
                if (rcue >= NOTE_MOY && strcmp(promotion->liste_etu[id_etu - 1].cursus[i].ue[k].code, "AJ") == 0)
                    strcpy(promotion->liste_etu[id_etu - 1].cursus[i].ue[k].code, "ADS");
        }
        printf("- %.1f (", rcue);
        affiche_code_rcue(promotion, rcue, id_etu, indice_sem, k);
        printf(") ");
    }
    printf("-");
    if (promotion->liste_etu[id_etu - 1].cursus[indice_sem].etat_semestre == AJOURNE) {
        printf(" ajourne");
    }
    else if (indice_sem == S6 - 1)
        affiche_etat_annee3(promotion, id_etu);
    printf("\n");
}

// fonction qui affiche le cursus de l'etudiant entre par l'utilisateur
void affiche_cursus(Promotion* promotion) {
    int id_saisie;
    scanf("%d", &id_saisie);
    if (estEnregistre(promotion, id_saisie) == FALSE) {
        printf("Identifiant incorrect\n");
        return;
    }
    printf("%d %s %s\n", id_saisie, promotion->liste_etu[id_saisie - 1].Prenom, promotion->liste_etu[id_saisie - 1].Nom);
    for (int j = 0; j < MAX_SEMESTRE; ++j) {
        const Etat_semestre et = promotion->liste_etu[id_saisie - 1].cursus[j].etat_semestre;
        if (et == EN_COURS || et == FINI || et == DEMISSION || et == AJOURNE || et == DEFAILLANCE || et == DIPLOME) {
            printf("S%d ", promotion->liste_etu[id_saisie - 1].cursus[j].num_semestre);
            for (int i = 0; i < NBR_UE; ++i) {
                if (promotion->liste_etu[id_saisie - 1].cursus[j].ue[i].note != NOTE_INCONNUE) {
                    const float note = promotion->liste_etu[id_saisie - 1].cursus[j].ue[i].note;
                    printf("- %.1f (%s) ", floorf(note * 10.f) / 10.f, promotion->liste_etu[id_saisie - 1].cursus[j].ue[i].code);
                }
                else printf("- * (*) ");
            }
            if (et != AJOURNE && et != DIPLOME)
                affiche_etat_semestre(et);
            else printf("-\n");
            if ((et == FINI || et == AJOURNE) && (j == S2-1 || j == S4-1))
                affiche_annee(promotion, id_saisie, &j);
            if ((et == FINI || et == AJOURNE || et == DIPLOME) && j == S6-1)
                affiche_annee(promotion, id_saisie, &j);
        }
        if (et == AJOURNE)
            return;
    }
}

//fonction qui renvoie le derner semestre d'un etudiant i
int determine_denier_sem(const Promotion* promotion, int i) {
    int dernier_sem = 1;
    for (int s = 0; s < MAX_SEMESTRE; ++s) {
        if (promotion->liste_etu[i].cursus[s].etat_semestre != NON_COMMENCE)
            dernier_sem = s + 1;
        else s = MAX_SEMESTRE;
    }
    return dernier_sem;
}

//fonction qui affiche la liste complete des etudiants
void affiche_liste_etu(Promotion* promotion) {
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id != VIDE) {
            const int dernier_sem = determine_denier_sem(promotion, i);
            const Etat_semestre et = promotion->liste_etu[i].cursus[dernier_sem - 1].etat_semestre;
            printf("%d - %s %s - S%d ", promotion->liste_etu[i].id, promotion->liste_etu[i].Prenom, promotion->liste_etu[i].Nom, promotion->liste_etu[i].cursus[dernier_sem - 1].num_semestre);
            affiche_etat_semestre(et);
        }
    }
}

//fonction qui met un etudiant en demission/defaillance
void saisie_demission_defaillance(Promotion* promotion, Etat_semestre etat) {
    int id_saisie;
    scanf("%d", &id_saisie);
    if (estEnregistre(promotion, id_saisie) == FALSE) {
        printf("Identifiant incorrect\n");
        return;
    }
    int semestre = semestre_en_cours(promotion, id_saisie);
    if (semestre == VIDE || promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].etat_semestre != EN_COURS) {
        printf("Etudiant hors formation\n");
        return;
    }
    promotion->liste_etu[id_saisie - 1].cursus[semestre - 1].etat_semestre = etat;
    for (int i = semestre; i < MAX_SEMESTRE; ++i) {
        promotion->liste_etu[id_saisie - 1].cursus[i].etat_semestre = NON_COMMENCE;
    }
    if (etat == DEMISSION)
        printf("Demission enregistree\n");
    else if (etat == DEFAILLANCE)
        printf("Defaillance enregistree\n");
}

//fonction qui termine la formation de tout les etudiants au semestre 6
void finFormation(Promotion* promotion) {
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].cursus[S6 - 1].etat_semestre == FINI) {
            for (int j = 0; j < NBR_UE; ++j) {
                float rcue = calcul_rcue(promotion, i + 1, S6 - 1, j);
                if (rcue < NOTE_MOY) {
                    promotion->liste_etu[i].cursus[S6 - 1].etat_semestre = AJOURNE;
                    j = NBR_UE;
                }
            }
            if (promotion->liste_etu[i].cursus[S6 - 1].etat_semestre == FINI)
                promotion->liste_etu[i].cursus[S6 - 1].etat_semestre = DIPLOME;
        }
    }
}

// retourne 1 si l'etudiant peut passer en annee 3 et 0 s'il ne peut pas
int test_annee3(const Promotion* promotion, int id_etu) {
    for (int i = 0; i < NBR_UE; ++i) {
        if (strcmp(promotion->liste_etu[id_etu - 1].cursus[S1 - 1].ue[i].code, "AJ") == 0 ||
            strcmp(promotion->liste_etu[id_etu - 1].cursus[S2 - 1].ue[i].code, "AJ") == 0)
            return 0;
    }
    return 1;
}

//fonction qui valide les UE par ADS
void ads(Promotion* promotion, int semestre, int indice_etu) {
    for (int j = 0; j < NBR_UE; ++j) {
        float rcue = calcul_rcue(promotion, indice_etu + 1, semestre - 1, j);
        if (rcue < NOTE_MIN) continue;
        if (rcue >= NOTE_MOY) {
            for (int k = semestre - 1; k >= 0; --k) {
                float note_k = promotion->liste_etu[indice_etu].cursus[k].ue[j].note;
                if (note_k < NOTE_MOY && promotion->liste_etu[indice_etu].cursus[k].etat_semestre != DEFAILLANCE && strcmp(promotion->liste_etu[indice_etu].cursus[k].ue[j].code, "ADC") != 0) {
                    strcpy(promotion->liste_etu[indice_etu].cursus[k].ue[j].code, "ADS");
                }
            }
        }
    }
}

//fonction qui renvoie le nombre d'etudiants qui peuvent terminer le semestre
int etudiant_termine(Promotion* promotion, int semestre) {
    int compteur = 0;
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id == VIDE) continue;
        if (test_ajourne_etu(promotion, i + 1, semestre - 1)) {
            ajourne_etu(promotion, i + 1, semestre - 1);
            ++compteur;
        }
        else if (promotion->liste_etu[i].cursus[semestre - 1].etat_semestre == EN_COURS) {
            promotion->liste_etu[i].cursus[semestre - 1].etat_semestre = FINI;
            if (semestre < S6)
                initialiser_cursus(promotion, promotion->liste_etu[i].id, semestre + 1);
            ++compteur;
        }
        if (promotion->liste_etu[i].cursus[semestre - 1].etat_semestre == DEFAILLANCE ||
            promotion->liste_etu[i].cursus[semestre - 1].etat_semestre == DEMISSION)
            continue; // ne rien faire pour les etudiants defaillants ou qui ont demissiones
        ads(promotion, semestre, i);
    }
    return compteur;
}

//fonction qui renvoie 1 si l'on peut réaliser un JURY et 0 si non.
int test_jury(const Promotion* promotion, int semestre) {
    if (semestre < S1 || semestre > MAX_SEMESTRE) {
        printf("Semestre incorrect\n");
        return 0;
    }
    // verification qu'aucune note n'est manquante pour les etudiants en cours dans ce semestre
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id != VIDE && promotion->liste_etu[i].cursus[semestre - 1].etat_semestre == EN_COURS) {
            for (int j = 0; j < NBR_UE; ++j) {
                if (promotion->liste_etu[i].cursus[semestre - 1].ue[j].note == NOTE_INCONNUE) {
                    printf("Des notes sont manquantes\n");
                    return 0;
                }
            }
        }
    }
    return 1;
}

// fonction qui fait passer la promotion au semestre suivant lorsqu'on est en semestre pair
void jury_pair(Promotion* promotion, int semestre) {
    if (test_jury(promotion, semestre) == FALSE)
        return;
    int compteur = etudiant_termine(promotion, semestre);
    test_adc(promotion, semestre);
    if (semestre == S4) {
        for (int i = 0; i < promotion->nbEtudiant; ++i) {
            if (test_annee3(promotion, i + 1) == FALSE && promotion->liste_etu[i].cursus[S4 - 1].etat_semestre == FINI)
                ajourne_etu(promotion, i + 1, S4 - 1);
        }
    }
    if (semestre == S6)
        finFormation(promotion);
    printf("Semestre termine pour %d etudiant(s)\n", compteur);
}

// fonction qui fait passer les personnes �ligibles au semestre suivant de celui saisie
void jury_impair(Promotion* promotion, int semestre) {
    if (test_jury(promotion, semestre) == FALSE)
        return;
    int compteur = 0;
    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id != VIDE && promotion->liste_etu[i].cursus[semestre - 1].etat_semestre == EN_COURS) {
            promotion->liste_etu[i].cursus[semestre - 1].etat_semestre = FINI;
            if (semestre < S6)
                initialiser_cursus(promotion, promotion->liste_etu[i].id, semestre + 1);
            compteur++;
        }
    }
    printf("Semestre termine pour %d etudiant(s)\n", compteur);
}

// fonction qui determine si le semestre est pair ou impair ou affcihe un message d'erreur
void decision_jury(Promotion* promotion) {
    int fin_semestre;
    scanf("%d", &fin_semestre);
    if (S1 <= fin_semestre && fin_semestre <= S6) {
        if (fin_semestre % 2 == 1)
            jury_impair(promotion, fin_semestre);
        else
            jury_pair(promotion, fin_semestre);
    }
    else
        printf("Semestre incorrect\n");
}

// fonction qui affiche le bilan pour une annee donnee en parametre et qui compte pour chaque statu possible combien il y en a
// durant l'annee donnee
void fait_bilan(const Promotion* promotion, int annee) {

    int nb_dem = 0, nb_def = 0, nb_en_cours = 0, nb_aj = 0, nb_passe = 0;

    /* borne_min = premier semestre de l'annee (1,3,5)
       borne_max = dernier semestre de l'annee (2,4,6) */
    int borne_min = 2 * annee - 1;
    int borne_max = 2 * annee;

    for (int i = 0; i < promotion->nbEtudiant; ++i) {
        if (promotion->liste_etu[i].id == VIDE) continue;

        int const dernier_sem = determine_denier_sem(promotion, i); /* 1..6 */

        /* ne garder que les etudiants ayant atteint au moins un semestre de l'annee */
        if (dernier_sem < borne_min) continue;

        /* choisir le semestre de reference :
           - si l'etudiant est encore dans l'annee (dernier_sem <= borne_max) -> prendre dernier_sem
           - sinon il a depasse l'annee -> prendre borne_max (etat du semestre de l'annee consideree) */
        int sem_a_considerer = (dernier_sem > borne_max) ? borne_max : dernier_sem;

        Etat_semestre etat_etu = promotion->liste_etu[i].cursus[sem_a_considerer - 1].etat_semestre;

        if (etat_etu == DEMISSION)
            ++nb_dem;
        else if (etat_etu == DEFAILLANCE)
            ++nb_def;
        else if (etat_etu == EN_COURS)
            ++nb_en_cours;
        else if (etat_etu == AJOURNE)
            ++nb_aj;
        else if (etat_etu == FINI || etat_etu == DIPLOME)
            ++nb_passe;
    }
    printf("%d demission(s)\n%d defaillance(s)\n%d en cours\n%d ajourne(s)\n%d passe(s)\n",
        nb_dem, nb_def, nb_en_cours, nb_aj, nb_passe);
}

// fonction qui determine de quelle annee faut faire le bilan et qui appelle la fonction fait_bilan avec la bonne annee en parametre
// ou renvoi un message d'erreur si l'annee n'est pas correct
void decision_bilan(const Promotion* promotion) {
    int annee;
    scanf("%d", &annee);
    if (annee == ANNEE1)
        fait_bilan(promotion, ANNEE1);
    else if (annee == ANNEE2)
        fait_bilan(promotion, ANNEE2);
    else if (annee == ANNEE3)
        fait_bilan(promotion, ANNEE3);
    else
        printf("Annee incorrecte\n");
}

//fonction qui va s'occuper de lire les commandes de l'utilisateur
void lecture_commande(Promotion* promotion) {
    char continuer = TRUE, commande[MAX_CARACTERES];
    while (continuer == TRUE) {
        if (scanf("%s", commande) != 1) break;
        if (strcmp(commande, commande_exit) == 0)
            continuer = FALSE;
        else if (strcmp(commande, commande_inscrire) == 0)
            saisie_nom(promotion);
        else if (strcmp(commande, commande_cursus) == 0)
            affiche_cursus(promotion);
        else if (strcmp(commande, commande_note) == 0)
            saisie_note(promotion);
        else if (strcmp(commande, commande_etudiants) == 0)
            affiche_liste_etu(promotion);
        else if (strcmp(commande, commande_demission) == 0)
            saisie_demission_defaillance(promotion, DEMISSION);
        else if (strcmp(commande, commande_defaillance) == 0)
            saisie_demission_defaillance(promotion, DEFAILLANCE);
        else if (strcmp(commande, commande_jury) == 0)
            decision_jury(promotion);
        else if (strcmp(commande, commande_bilan) == 0)
            decision_bilan(promotion);
    }
}

int main() {
    Promotion promotion;
    promotion.nbEtudiant = 0;
    lecture_commande(&promotion);
}