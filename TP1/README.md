#TDP1
##Compilation
*`make timing`: génération de l'executable générant les courbes de perf
*`make drive`: génération de l'executable générant les tests de validation

##Exécution
*`./timing`: les fichiers .dat contenant les données pour tracer les courbes sont générés dans `curves/data`. Il faut ensuite executer les différent script gnuplot présents dans `curves` pour créer les courbes.
*`./driver`: une série de tests permettant la validation des fonctions blas sont lancés.
