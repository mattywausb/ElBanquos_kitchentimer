
[Back to mainpage](https://mattywausb.github.io/ElBanquos_kitchentimer)
___
# Elemente und Aufbau des Gerätes
Elbanquos Kitchentimer besteht aus folgenden Elementen:
* 8 stellige Ziffenanzeige, je nach Modus 2 Ziffern zu jedem Timer oder für 8 stelliges Informationen 
* Je Timer ein dedizierter Taster
* Je Timer eine Timer-LED (eingebaut in den Taster)
* Zentraler Drehregler mit Tastfunktion

![The timer](build_pictures/08_final_device.jpg)

# Kurzanleitung
Um einen Timer zu **aktivieren** ist folgendes zu tun
* Mit dem  ***Drehregler*** die gewünschte Zeit einstellen (beginnt bei **10 Minuten**)
* Taster eines inaktiven (LED Grün oder aus) Timers drücken ->  Der Timer startet. Der Timer Taster leuchtet dauerhaft rot.

Um einen **Timer mit einer voreingestellte Zeit** zu aktivieren:
* ***Taster*** des gewünschten Timers ***drücken*** -> die Vorgabezeit des jeweiligen Timers wird angezeigt und der Taster blinkt grün
* Mit dem ***Drehregler*** bei Bedarf die Zeit anpassen
* Um die Zeit ggf. auf einem anderen freien Timer zu legen, ***anderen Timer Taster drücken***
* ***Taster des Timers oder den Drehregler drücken*** -> Der Timer startet. Der Timer Taster leuchtet dauerhaft rot.

Um bei einem aktiven Timer die **verbleibende Zeit exakt abzulesen**:
* ***Taster des gewünschten Timers drücken*** -> Zeit wird in allen Details angezeigt
* ***Taster des Timers erneut drücken*** -> es werden wieder alle Timer angezeigt

Um die Zeit eines bereits **laufenden Timers zu ändern**:
* ***Taster des gewünschten Timers drücken*** -> Zeit wird in allen Details angezeigt
* Mit dem ***Drehregler*** die Zeit anpassen
* ***Taster des Timers*** drücken ***oder den Drehregler kurz drücken***

Ist ein **Timer abgelaufen** ertönt ein Signalton und Taster und Anzeige des Timers blinken sehr schnell.
* **Alarm beenden** Durch ***drücken des blinkenden Tasters***
* Alternativ schaltet sich der Alarm nach 45 Sekunden von selbst ab sofern nicht ein weitere Timer abläuft. 

Im **Nachlaufmodus** zählt der Timer die Zeit, die mit Beginn des Alarms verstrichen ist. Der Timer-Taster ist dunkel und im Display wird die seit dem Alarm verstrichene Zeit abwechselnd mit "\_ \_" angezeigt. Ab 2 Minuten wird nur noch "\_ \_" angezeigt. Die **seit dem Alarm verstrichende Zeit** kann durch ***Drücken des Tasters*** abgerufen werden. 

Um einen Timer (egal ob abgelaufen oder aktiv) zu **deaktivieren** ist folgendes zu tun:
* ***Taster des Timers 5 Sekunden halten*** -> Für den Timer wird kurz "OF angezeigt und der Timer ist danach wieder inaktiv (grün). Ein abgelaufender Timer deaktiviert sich selbst nach der Hälfte seiner ursprünglichen Laufzeit. (Frühestens allerding nach 3 Minuten und spätestens nach 90 Minuten)

Weitere Funktionen werden im Abschnitt "Die Fuktionen im einzelnen" beschrieben.

# Die Zeitanzeige im Detail
## Hauptanzeige (Kompakte Darstellung aller Timer parallel)
Auf der Hauptanzeige werden die Zeiten von allen Timern parallel nebeneinander dargestellt. Für jeden Timer sind zwei Ziffern der Anzeige passend zur Position des Tasters und der Timer-Led vorgesehen. Die Darstellung ist wie folgt:
* **Zeitinformation**
    * "zz" (Zwei Ziffern, ohne Punkt, sekündlich abwärtslaufend) - verbleibende Sekunden zz bis zum Alarm 
    * "y'z" (Zwei Ziffern, mit einem Punkt oben getrennt) - mehr als 60 Sekunden Zeit: Vor dem Punkt : Verbleibende Minuten y, Nach dem Punk verbeibende 10 Sekundenschritte
    * "'yy" (Zwei Ziffern, denen ein Punkt vorangestellt ist) - mehr als 10 Minuten Zeit: Verbleibende Minuten
    * "xh" - (eine Ziffer gefolgt von einem kleinen h und einem Punkt) -  mehr als 1 Stunde Zeit: Verbleibende Stunden x
    * "Hx'" - (Ein großes H gefolgt von einer Ziffer und einem Punkt) -  mehr als 10 Stunden Zeit: Verbleibende 10 Stundenschritte
    * "vd'" - (eine Ziffer gefolgt von einem d und einem Punkt) -  mehr als 24 Stunden Zeit: verbleibende Tage
* **Statusinformation**
	* keine Anzeige - Der Timer ist nicht aktiv
    * "\_ \_"  = Der Timer ist im Nachlaufmodus
    * "\_ \_"  Wechselnd zu einer der Zeitinformation = Der Alarm ist keine 2 Minuten her
    * "°_" und "_°" = Timer Alarm in schnellem Wechsel mit einer Zeitinformation
	* "P" vor der Zeitanzeige: Die Zeiteinstellung erfolgt in Relation zu einem Partnertimer
	* "A" vor der Zeitanzeige: Es wird die ursprünglich eingestellte Zeit des Timers angezeigt

Beispiele:
* 12 = 12 Sekunden 
* 5'3 = 5 Minuten und 30-39 Sekunden
* 24' = 24 Minuten
* 2h  = Zwischen 2 Stunden und 2 Stunden, 59 Minuten, 59 Sekunden
* 4d  = Mindestenst 4 Tage, weniger als 5 Tage

![Example](display_examples/t149_t28_1_t31h_t429.jpg)

* Timer 1 hat noch 49 Sekunden
* Timer 2 hat noch 8 Minuten und mindestens 10 Sekunden
* Timer 3 hat noch mehr als 1 Stunde (aber weniger als 2)
* Timer 4 hat noch 29 Minuten
 
## Anzeige einer einzelnen Zeit
Bei der Auswahl eines Timers und für die Einstellung der Zeit gibt zwei Formate der Darstellung:

* xx**h**'yy'zz - Für alle Werte unter 24 Stunden wird die Stunde x, die Minuten yy und die Sekunden zz angezeigt
* v**d**xx**h**'yy - Wert größer 24 Stunden werden mit der Anzahl Tage v, die Stunden xx und Minuten yy angezeigt.

Beispiel:
![Example](display_examples/detail_h_to_s_1h4435.jpg)

## Information durch die LED im Timer-Taster
* LED **grün** durchgehend: Der Timer ist ***inaktiv*** 
* LED **blinkt regelmäßig**: Der Timer ist ***ausgewählt*** für die Detailsanzeige oder Zeiteinstellung. Sollten zwei Timer blinken, so wird der grün blinkende Timer in Relation zu dem rot blinkenden eingestellt. 
* LED **rot** durchgehend: Der Timer ***läuft***
* LED **rot schnell blinkend** mit kurzen Pausen: Der Timer hat einen ***Alarm***
* LED **rot** erlischt kurz im **"Herzschlagryhtmus"**: Der Timer ist im ***Pausemodus***
* LED **aus**: Der Alarm für den Timer wurde beendet und der Timer ist im ***Nachlaufmodus***. Er steht für einen neuen Start zur Verfügung. 


# Die Funktionen im Einzelnen
Alle Funktionen werden hier ausgehend von der Hauptanzeige(siehe oben) beschrieben. Diese erreicht man im Zweifelsfall immer durch kurzes Warten (15 Sekunden).

## Timer einstellen und starten
Anmerkung vorab: Um auch große Zeitintervalle effizient erreichen zu können wächst der Zeitabstand pro Schritt des Drehregles mit der insgesamt gewählten Zeit. Dadurch kann nicht für jedes Intervall eine sekundengenaue Einstellung erfolgen (Weitere Details dazu im Anhang "Stufung der Einstellintervalle"). 

### Per Drehregler
* Mit dem Drehregler eine Zeit einstellen (Die erste Bewegung des Reglers setzt diese auf 10 Minuten).
* Taster eines nicht laufenden Timers (_LED grün oder aus_) drücken -> Der Timer wird gestartet.

Solange noch kein Timer ausgewählt wurde, kann die Eingabe durch langes Drücken des Drehreglers abgebrochen werden. 
Die Eingabe wird ebenfalls abgebrochen, wenn man 15 Sekunden lang keine Bedienhandlung vornimmt.

### Direkte Zeitvorwahl je Timer
* Taster eines deaktivierten(LED ist grün) Timers drücken -> die für den Timer vorprogrammierte Vorschlagszeit erscheint,Timer-Led blinkt
* Mit dem Drehregler die Zeit anpassen
* Timer starten indem man auf den Drehregler drückt _oder_ erneut die Taste des Timers betätigt
* Option: Drückt man stattdessen die Taste eines anderen inaktiven Timers, wird auf diesen als Ziel für die eingestellte Zeit umgeschaltet. Ein erneuter Druck auf dessen Taster oder den Drehknopf startet diesen Timer. Der ursprüngich ausgewählte Timer bleibt dann unberührt.

Die Eingabe durch langes drücken des Drehreglers abgebrochen werden. 
Die Eingabe wird ebenfalls abgebrochen, wenn man 15 Sekunden keine Bedienhandlung vornimmt.

### Zeitvorwahl mit abgelaufenem Timer
* Taster eines nachlaufenden(LED ist aus) Timers drücken -> die Nachlaufzeit des Timers wird angezeigt,Timer-Led blinkt
* Die erste Bewegung des Drehreglers stellt die Zeit auf die vorprogrammierte Vorschlagszeit
* Danach kann die Zeit weiter angepasst werden
* Timer starten indem man auf den Drehregler drückt _oder_ erneut die Taste des Timers betätigt
* Option:Drückt man stattdessen die Taste eines anderen inaktiven Timers, wird auf diesen als Ziel für die eingestellte Zeit umgeschaltet. Ein erneuter Druck auf dessen Taster oder den Drehknopf startet diesen Timer. Der ursprüngich ausgewählte Timer bleibt dann unberührt.

### Zeitabstand zu laufendem Timer
Möchte man einen Timer stellen, der eine definierte Zeit __vor__ einem schon laufenden Timer abläuft, ist wie folgt zu verfahren:
* Taster des aktiven Timers, für zu dem man einen Abstand festlegen will drücken und halten. 
* zusätzlich Taster des inaktiven Timers drücken, der vor dem schon aktiven Timer ablaufen soll -> Der Taster des aktiven Timers blinkt rot, der Taster des einzustellenden Tasters blinkt grün. In der Zeitanzeige wird die Hälfte der Restzeit des aktiven Timers vorgeschlagen. Mit einem "P" in der Anzeige wird angezeigt, dass der "Partnermodus" aktiviert ist
* Mit dem Drehregler die Restzeit einstellen, die der aktive Timer haben soll, wenn der jetzt eingestellte Timer abläuft.
* Den zweiten Timer starten, indem man den Drehregler drückt oder den Taster des zweiten Timers->Der zweiten Timer erhält dabei eine Laufzeit, die in Relation zu dem partner mit dem eingestellten Abstand abläuft. 
Sollte die Zeit des Ursprungstimers nicht mehr ausreichen, um die gewünschte Zeit zu realisieren, wird der Timer nicht gestartet. Stattdessen wird kurz ein Fehler angezeigt und danach die Ursprungtimer wieder angezeigt. So kann bei Bedarf, dessen Zeit veändert werden, oder das Prozedere für einen zweiten Timer erneut begonnen werden.

Die Timer sind danach nicht weiter verkoppelt, d.h. wenn der erste Timer pausiert oder verändert wird, hat das keine Auswirkung auf den zweiten Timer. Der ürsprünglich gewünschte Zeitabstand ist dann nicht mehr gegeben.


## Timer Restzeit anzeigen
Um die exakte Restzeit eines Timers anzuzeigen:
* Taster des gewünschten aktiven Timers drücken -> Ausführliche Restzeit (oder die Zeit seit dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Durch erneutes Drücken des Tasters wird wieder die Hauptanzeige angezeigt
* Alternativ: Durch Drücken des Tasters eines anderes aktiven Timers wird zu dessen Anzeige umgeschaltet.

## Timer pausieren
Um einen Timer anzuhalten:
* Taster des gewünschten laufenden Timers drücken -> Ausführliche Restzeit wird angezeigt. Die Timer-Led blinkt.
* Den Drehregler kurz drücken -> Auf der Position des Timers wird kurz "PA" angezeigt. Danach erscheint die Hauptanzeige und die Ziffern des Timers, sowie dessen Timer-Led blinken mit einem langsamen "Herzschlagryhtmus"

## Pausierten Timer wieder aktivieren
Um einen pausierten Timer wieder zu aktivieren:
* Taster des pausierten Timers drücken -> Ausführliche Restzeit  wird angezeigt. Die Timer-Led blinkt.
* Den Drehregler kurz drücken -> Auf der Positin des Timers wird kurz "Go" angzeigt. Danach der erscheint die Hauptanzeige und der Timer wird wieder durchgehend angezeigt.

## Timer Restzeit ändern
Um die Restzeit eines Timers anzupassen:
* Taster des gewünschten laufenden Timers drücken -> Ausführliche Restzeit (oder die Zeit seit dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Mit dem Drehregler die Zeit entsprechend anpassen 
* Neue Zeit mit Druck auf Drehregler oder den Taster des Timers bestätigen-> Hauptanzeige mit der geänderten Zeit wird angezeigt. War der Timer vorher pausiert, ist er jetzt wieder aktiv 
* Alternativ: Änderung durch Drücken und halten des Drehreglers verwerfen

## Timer Startzeit anzeigen
Um die ursprüngliche Startzeit des Timers anzuzeigen:
* Taster des gewünschten aktiven Timers drücken -> Ausführliche Restzeit (oder die Zeit seit nach dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Drehregler drücken und halten -> Die ursprüngliche Startzeit wird angezeigt bis man den Drehregler wieder loslässt
* Taster des Timers erneut drücken um wieder in die Anzeige aller Timer zu kommen oder 15 Sekunden abwarten

## Timer Alarm bestätigen
Wenn ein Timer im Alarmmodus ist, kann dieser deaktiviert werden in dem man
* den Taster des Timers einmal betätigt
Dies funktioniert in jeder Bediensituation.

## Timer deaktivieren
Um einen Timer vorzeitig abzuschalten, oder nach Ablauf der Zeit dessen Nachlaufmessung zu beenden:
* Taster des Timer drücken und für 5 Sekunden halten -> Auf der Positin des Timers wird kurz "OF" angzeigt. Danach der erscheint die Hauptanzeige und der Timer ist deaktiviert.

Ein Timer deaktiviert sich automatisch nach einer Nachlaufzeit, die dem doppelten seiner Start entspricht +45 Sekunden, maximal aber 90 Minuten . Diese Regel entspringt dem Gedanken, dass spätestens nach der doppelt eingestellten Zeit die zu messende Aktivität entweder komplett erledigt oder aus dem Ruder gelaufen ist. 

## Timer kopieren
Es ist möglich von einem laufenden Timer die Zeit zu "Kopieren".
* Taster des gewünschten laufenden Timers drücken von dem die Zeit kopiert werden soll -> Ausführliche Restzeit (oder die Zeit seit nach dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Mit dem Drehknopf eine (ggf. auch sehr kleine) Änderung der Zeit vornehmen
* Taster des "Ziel"-Timers, auf den der Wert kopiert werden soll drücken (muss ein freier Timer sein) ->Die Timer-Led des anvisierten Timers blinkt.
* Taster des "Ziel"-Timers erneut drücken -> Der Timer startet mit der entsprechenden Zeit. Der Timer von dem die Zeit kopiert wurde, verbleibt in seinem vorigen Zustand.

# Der "Melodiecode" im Alarmton
Die Melodie des Alarmtons richtet sich nach dem alarmgebenden  Timer wie folgt:
* Grundmelodie sind 5 gleiche Töne gefolgt von einer 3 Noten langen Pause
* Der zweite bis vierte Ton der 5 Töne wird je nach alarmieredem Timer anhgehoben. Die Anzahl der Töne vor dem angehobenen Ton gibt die Position des Timers auf dem Bedienfeld an. Sind mehrere Timer im Alarmzustand werden entsprechend alle zugehörigen Töne angehoben.

# Unterschiedliche Genauigkeitsstufung 
Da bei der Zeitmessung für größere Intervalle eine geringere Genauigkeit akzeptabel ist (zumindest in einer normalen Küche) wird die Einstellgenauigkeit gröber mit der Größe der Gesamtzeit. So bleibt die Anzahl der notwendigen Umdrehungen des Drehknopfes auch für sehr große Intervalle überschaubar (Hallo Sauerteigbrotbäcker und Soßenspezialköche).

    Zeit   Genauikeit
    <30 s    1 s
    <2 min   5 s
    <5 min   10 s
    <15 min  15 s
    <30 min  30 s
    <1 h     1 min
    <3 h     10 min
    <24 h    15 min
    <48 h    30 min
    >48 h     1 h




