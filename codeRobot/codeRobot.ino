//
// Hackteliers @ Lycée Le Corbusier             / Fevrier  2015
// Atelier arduino @ Mediathèque de Hautepierre / Octobre  2015
// Atelier arduino @ Mediathèque de Neudorf     / Novembre 2015
//
// Code pour controller le robot
//

#include <Servo.h>

// ###############################
// #  Configuration des roues    #
// ###############################

// Attribution des pins correspondant aux roues
#define PIN_ROUE_GAUCHE 9
#define PIN_ROUE_DROITE 10

// Valeurs mesurées sur le robot
#define RAYON_ROUES      33
#define ECARTEMENT_ROUES 110

// Tours par seconde observés sur le robot par rapport à la vitesse par défaut donnée
#define VITESSE_PAR_DEFAUT 70
#define TOUR_PAR_SECONDE 0.528

// ################################
// # CORRECTION AVANCER + TOURNER #
// ################################

// Correction pour la fonction avancer si légère asymétrie entre gauche et droite
#define CORRECTION_AVANCER_GAUCHE 0
#define CORRECTION_AVANCER_DROITE 2

// Correction pour la fonction tourner
#define CORRECTION_TOURNER 0.42

// ###############################
// # Gestion des moteurs / roues #
// ###############################

// Centre des roues ou celles-ci ne bougent plus
#define CENTRE_ROUE_GAUCHE 1500
#define CENTRE_ROUE_DROITE 1500

// Déclaration des servomoteurs
Servo servoRoueGauche;
Servo servoRoueDroite;

// Fonction d'attribution et d'initialisation des servomoteurs
void initRoues()
{
    servoRoueGauche.attach(PIN_ROUE_GAUCHE);
    servoRoueDroite.attach(PIN_ROUE_DROITE);
	commandeRoues(0,0);
}

// Fonction de désactivation des roues afin d'économiser de l'énergie
void desinitRoues()
{
    servoRoueGauche.detach();
    servoRoueDroite.detach();
}

// Fonction de base permettant de faire tourner les roues en leur donnant une valeur 
// en microsecondes.. Si supérieure à 1500 avance, si inférieure, recule
void commandeRoues(int commandeGauche, int commandeDroite)
{
    servoRoueGauche.writeMicroseconds(CENTRE_ROUE_GAUCHE+commandeGauche);
    servoRoueDroite.writeMicroseconds(CENTRE_ROUE_DROITE+commandeDroite);
}

// Fonction permettant de rapidement, mais progressivement accélerer les roues après l'éxecution 
// d'une fonction avancer() ou tourner() afin d'éviter une variation soudaine de tension faisant 
// rebooter l'Arduino.
void startRouesSlow(int comG, int comD)
{
    for (float f = 0.0 ; f <= 0.9 ; f += 0.1)
    {
        commandeRoues(comG * f,comD * f);
        attendre(50);
    }
    commandeRoues(comG, comD);
}

// Idem que la fonction précédente mais pour les ralentir progressivement
void stopRouesSlow(int comG, int comD)
{
    for (float f = 0.9 ; f >= 0.0 ; f -= 0.1)
    {
        commandeRoues(comG * f,comD * f);
        attendre(50);
    }
    commandeRoues(0,0);
    attendre(100);
}

// Fonction permettant d'attendre une durée donnée
// (pendant ce temps, les roues continuent de tourner)
void attendre(unsigned long duree)
{
    delay(duree);
}

// ######################
// #  Fonction avancer  #
// ######################

// Calcul de la distance par seconde
#define DISTANCE_PAR_SECONDE ((float) (2 * 3.1415 * RAYON_ROUES * TOUR_PAR_SECONDE))

// Fonction pour avancer prenant en argument une distance demandée 
void avancer(int distance)
{
    // Determine si la valeur de la distance est positive ou négative afin de définir s'il faut
    // avancer ou reculer
    short int sens = (distance > 0) ? 1 : -1;

    // Calcule la durée nécessaire pour parcourir la distance demandée
    float duree = abs((float) distance) / DISTANCE_PAR_SECONDE;
    
    // Définie la valeur à donner à la fonction commandeRoue afin d'avancer, reculer ou tourner
    int comG = CORRECTION_AVANCER_GAUCHE + sens * VITESSE_PAR_DEFAUT;
    int comD = CORRECTION_AVANCER_DROITE - sens * VITESSE_PAR_DEFAUT;

	// Fait démarrer progressivement les roues jusqu'a la vitesse standard
    startRouesSlow(comG, comD);
    // la fonction attendre permet de faire perdurer l'action de commandeRoue
    attendre(duree * 1000);
    // Excute la fonction permettant de ralentir la rotation des roues 
    stopRouesSlow(comG,comD);
}

// ######################
// #  Fonction tourner  #
// ######################

// Fonction permettant de faire tourner les roues dans des sens opposés afin d'opérer une rotation
void tourner(float fraction)
{
    // Défini un sens de rotation par rapport à la valeur demandée
    short int sens = (fraction > 0) ? 1 : -1;

    // Défini la durée nécessaire pour accomplir la rotation demandée
    float duree =  abs((float) fraction) * 3.1415 * (float) (ECARTEMENT_ROUES) / DISTANCE_PAR_SECONDE;
    duree -= CORRECTION_TOURNER;
    
    // Défini la commande à appliquer sur les moteurs
    int com = sens * VITESSE_PAR_DEFAUT;
    
    // Execute la fonction commande roue avec la meme valeur pour chaque roue, celles-ci étant
    // inversées sur le robot, l'une semblera tourner dans un sens et l'autre dans l'autre
    commandeRoues(com, com);
    attendre(duree * 1000);
    stopRouesSlow(com,com);
}

// ###############################
// #  Le programme               #
// ###############################

void setup()
{
    initRoues();
}

void loop()
{
    avancer(500);
    attendre(1000);
    tourner(0.25);
    attendre(1000);
}




