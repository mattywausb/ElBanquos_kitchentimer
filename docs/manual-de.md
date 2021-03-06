
[Back to mainpage](https://mattywausb.github.io/ElBanquos_kitchentimer)
___
# Elemente und Aufbau des Gerätes
Elbanquos Kitchentimer besteht aus folgenden Elementen:
* 8 stellige Ziffenanzeige, je nach Modus 2 Ziffern zu jedem Timer oder für 8 stelliges Informationen 
* Je Timer ein dedizierter Taster
* Je Timer eine Timer-LED (eingebaut in den Taster)
* Zentraler Drehregler mit Tastfunktion

![vision_1](vision_1.jpg "Concept")

# Schnellstart
Um einen Timer zu **aktivieren** ist folgendes zu tun
* Mit dem zentralen Drechknopf die gewünschte Zeit einstellen
* Taster eines inaktiven* Timers drücken ->  der Timer startet, auf der Displayposition des Timers erscheint kurz "Go",  im Display wird die Restzeit auf der Position des Timers angezeigt, die Timer-LED leuchtet dauerhaft rot.

**Der Timertaster leuchtet grün oder ist nicht beleuchtet.
Der Alarm kann mit einem Druck auf den Taster des Timers bestätigt werden und wird damit abgeschaltet. Nach 45 Sekunden schaltet sich der Alarm von selbst ab. 

Der Timer befindet sich danach im **Nachlaufmodus**. Die der Timer-Taster leuchtet nicht und im Display wird die, seit dem Alarm verstrichene Zeit, abwechselnd mit "_._" angezeigt. Ab 2 Minuten wird nur noch "_._" angezeigt. Mittels des Timer-Tasters kann die seit dem Alarm verstrichende Zeit abgerufen werden. 

Um den Timer komplett zu **deaktivieren** ist folgendes zu tun:
* Taster des Timers 5 Sekunden halten -> Für den Timer wird kurz "OF angezeigt und der Timer ist danach wieder im Ruhezustand (grün)

# Die Zeitanzeige im Detail
## Hauptanzeige (Kompakte Darstellung aller Timer parallel)
Auf der Hauptanzeige werden die Zeiten von allen Timern parallel nebeneinander dargestellt. Für jeden Timer sind zwei Ziffern der Anzeige passend zur Position des Tasters und der Timer-Led vorgesehen. Die Darstellung ist wie folgt:
* Zeitinformation
    * "zz" (Zwei Ziffern, ohne Punkt, sekündlich abwärtslaufend) - verbleibende Sekunden zz bis zum Alarm 
    * "y.z" (Zwei Ziffern, mit einem Punkt getrennt) - mehr als 60 Sekunden Zeit: Vor dem Punkt : Verbleibende Minuten y, Nach dem Punk verbeibende 10 Sekundenschritte
    * "yy." (Zwei Ziffern, gefolgt von einem Punkt) - mehr als 10 Minuten Zeit: Verbleibende Minuten
    * "xh." - (eine Ziffer gefolgt von einem kleinen h und einem Punkt) -  mehr als 1 Stunde Zeit: Verbleibende Stunden
    * "Hx." - (Ein großes H gefolgt von einer Ziffer und einem Punkt) -  mehr als 10 Stunden Zeit: Verbleibende 10 Stundenschritte
    * "vd." - (eine Ziffer gefolgt von einem d und einem Punkt) -  mehr als 24 Stunden Zeit: verbleibende Tage

* Statusinformation
    * nur ein "." = Der Timer ist deaktiviert
    * "_._" = Der Timer ist im Nachlaufmodus
    * "_._" Wechselnd zu einer der Zeitinformation = Der Alarm ist keine 2 Minuten her
    * "°_" und "_°" = Timer Alarm 

Beispiele:
* 12 = 12 Sekunden 
* 5.3 = 5 Minuten und 30-39 Sekunden
* 24. = 24 Minuten
* 2h  = Zwischen 2 Stunden und 2 Stunden, 59 Minuten, 59 Sekunden
* 4d  = Mindestenst 4 Tage, weniger als 5 Tage

* 33.2d . 4.5   = 
    * timer 1 hat noch 33 sekunden
    * Timer 2 hat noch zwischen 2 und 3 Tagen
    * Timer 3 ist deaktivert
    * Timer 4 hat noch 4 Minuten und mindestens 50-59 Sekunden
 
## Anzeige einer einzelnen Zeit
Bei der Auswahl eines Timers und für die Einstellung der Zeit gibt zwei Formate der Darstellung:

* xxhyy.zz - Für alle Werte unter 24 Stunden wird die Stunde x, die Minuten yy und die Sekunden zz angezeigt
* vdxxhyy. - Wert größer 24 Stunden werden mit der Anzahl Tage v, die Stunden xx und Minuten yy angezeigt.

Beispiele:
* 14h22.58 = 14 Stunden, 22 Minuten, 58 Sekunden 
* 3d 7h29. = 3 Tage, 7 Stunden, 29 Minuten

## Information durch die Timer-LED im Timer-Taster
* LED dauerhaft **grün**: Der Timer ist deaktiviert oder im Nachlaufmods und steht zum setzen einer neuen Startzeit zur Verfügung 
* LED dauerhaft **rot**: Der Timer **läuft**
* LED **blinkt schnell** mit kurzen Pausen: Der Timer hat einen **Alarm**
* LED **blinkt regelmäßig**: Der Timer ist gerade zur Detailanzeige oder Einstellung **ausgewählt**
* LED in **rot** erlischt kurz im **"Herzschlagryhtmus"**: Der Timer ist im **Pausemodus**

# Die Funktionen im Einzelnen
Alle Funktionen werden hier ausgehend von der Hauptanzeige(siehe oben) beschrieben. Diese erreicht man im Zweifelsfall immer durch kurzes Warten (15 Sekunden).
## Timer einstellen und starten
Anmerkung vorab: Um auch große Zeitintervalle effizient erreichen zu können passt sich das Einstellintervall situativ an. Dadurch kann nicht für jedes Intervall eine sekundengenaue Einstellung erfolgen (Weitere Details dazu im Anhang "Stufung der Einstellintervalle"). 

### Variante 1 (Schnellstart)
* Mit dem **Drehregler** eine Zeit einstellen (Die erste Bewegung des Reglers setzt diese auf 10 Minuten).
* **Taster** eines **nicht laufenden** Timers (_LED grün oder aus_) **drücken** -> Der Timer wird gestartet.

Solange noch kein Timer ausgewählt wurde, kann die Eingabe durch langes Drücken des Drehreglers abgebrochen werden. 
Die Eingabe wird ebenfalls abgebrochen, wenn man 15 Sekunden lang keine Bedienhandlung vornimmt.

### Variante 2
* **Taster** eines **deaktivierten**(LED ist grün) Timers drücken -> die für den Timer _vorprogrammierte Vorschlagszeit_ erscheint,Timer-Led blinkt
* Mit dem **Drehregler** die Zeit anpassen
* Timer starten indem man auf den **Drehregler** drückt _oder_ erneut die **Taste** des Timers betätigt
* Option: Drückt man stattdessen die Taste eines anderen inaktiven Timers, wird auf diesen als Ziel für die eingestellte Zeit umgeschaltet. Ein erneuter Druck auf dessen Taster oder den Drehknopf startet diesen Timer. Der ursprüngich ausgewählte Timer bleibt dann unberührt.

Die Eingabe durch langes drücken des Drehreglers abgebrochen werden. 
Die Eingabe wird ebenfalls abgebrochen, wenn man 15 Sekunden keine Bedienhandlung vornimmt.

### Variante 3
* **Taster** eines **nachlaufenden**(LED ist aus) Timers drücken -> die Nachlaufzeit des Timers wird angezeigt,Timer-Led blinkt
* Die erste Bewegung des **Drehreglers** stellt die Zeit auf die __vorprogrammierte Vorschlagszeit_
* Danach kann die Zeit weiter angepasst werden
* Timer starten indem man auf den **Drehregler** drückt _oder_ erneut die **Taste** des Timers betätigt
* Option:Drückt man stattdessen die Taste eines anderen inaktiven Timers, wird auf diesen als Ziel für die eingestellte Zeit umgeschaltet. Ein erneuter Druck auf dessen Taster oder den Drehknopf startet diesen Timer. Der ursprüngich ausgewählte Timer bleibt dann unberührt.

## Timer Restzeit anzeigen
Um die exakte Restzeit eines Timers anzuzeigen:
* **Taster** des gewünschten **aktiven** Timers **drücken** -> Ausführliche Restzeit (oder die Zeit seit dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Durch erneutes Drücken des Tasters wird wieder die Hauptanzeige angezeigt
* Alternativ: Durch Drücken des Tasters eines anderes aktiven Timers wird zu dessen Anzeige umgeschaltet.

## Timer pausieren
Um einen Timer anzuhalten:
* **Taster** des gewünschten **laufenden** Timers **drücken** -> Ausführliche Restzeit wird angezeigt. Die Timer-Led blinkt.
* Den **Drehregler kurz drücken** -> Auf der Position des Timers wird kurz "PA" angezeigt. Danach erscheint die Hauptanzeige und die Ziffern des Timers, sowie dessen Timer-Led blinken mit einem langsamen "Herzschlagryhtmus"

## Pausierten Timer wieder aktivieren
Um einen pausierten Timer wieder zu aktivieren:
* **Taster** des **pausierten** Timers drücken -> Ausführliche Restzeit  wird angezeigt. Die Timer-Led blinkt.
* Den **Drehregler** kurz **drücken** -> Auf der Positin des Timers wird kurz "Go" angzeigt. Danach der erscheint die Hauptanzeige und der Timer wird wieder durchgehend angezeigt.

## Timer Restzeit ändern
Um die Restzeit eines Timers anzupassen:
* **Taster** des gewünschten **laufenden** Timers drücken -> Ausführliche Restzeit (oder die Zeit seit dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Mit dem **Drehregler** die Zeit entsprechend anpassen 
* Neue Zeit mit Druck auf **Drehregler** oder den **Taster** des Timers bestätigen-> Hauptanzeige mit der geänderten Zeit wird angezeigt. War der Timer vorher pausiert, ist er jetzt wieder aktiv 
* Alternativ: Änderung durch Drücken und halten des Drehreglers verwerfen

## Timer Startzeit anzeigen
Um die ursprüngliche Startzeit des Timers anzuzeigen:
* **Taster** des gewünschten **aktiven** Timers drücken -> Ausführliche Restzeit (oder die Zeit seit nach dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* **Drehregler** drücken und **halten** -> Die ursprüngliche Startzeit wird angezeigt bis man den Drehregler wieder loslässt
* Taster des Timers erneut drücken um wieder in die Anzeige aller Timer zu kommen oder 15 Sekunden abwarten

## Timer Alarm bestätigen
Wenn ein Timer im Alarmmodus ist, kann dieser deaktiviert werden in dem man
* den **Taster** des Timers einmal betätigt
Dies funktioniert in jeder Bediensituation.

## Timer deaktivieren
Um einen Timer vorzeitig abzuschalten, oder nach Ablauf der Zeit dessen Nachlaufmessung zu beenden:
* **Taster** des Timer drücken und für **5 Sekunden halten** -> Auf der Positin des Timers wird kurz "OF" angzeigt. Danach der erscheint die Hauptanzeige und der Timer ist deaktiviert.

Ein Timer deaktiviert sich automatisch nach einer Nachlaufzeit, die dem doppelten seiner Start entspricht +45 Sekunden, maximal aber 90 Minuten . Diese Regel entspringt dem Gedanken, dass spätestens nach der doppelt eingestellten Zeit die zu messende Aktivität entweder komplett erledigt oder aus dem Ruder gelaufen ist. 

## Timer kopieren
Es ist möglich von einem laufenden Timer die Zeit zu "Kopieren".
* **Taster** des gewünschten laufenden Timers drücken von dem die Zeit kopiert werden soll -> Ausführliche Restzeit (oder die Zeit seit nach dessen Ablauf) wird angezeigt. Die Timer-Led blinkt.
* Mit dem Drehknopf eine (ggf. auch sehr kleine) Änderung der Zeit vornehmen
* **Taster des "Ziel"-Timers**, auf den der Wert kopiert werden soll drücken (muss ein freier Timer sein) ->Die Timer-Led des anvisierten Timers blinkt.
* Taster des **"Ziel"-Timers** erneut drücken -> Der Timer startet mit der entsprechenden Zeit. Der Timer von dem die Zeit kopiert wurde, verbleibt in seinem vorigen Zustand.


# Anhang
## Stufung der Einstellungsintervalle
Um auch Intervalle von bis zu 7 Tagen einstellen zu können, sind zwei Methoden der Skalierung der Einstellungsstufen implementiert.
### Fest definierte Genauigketsstufung
Da bei der Zeitmessung für größere Intervalle eine geringere Genauigkeit akzeptabel ist (zumindest in einer normalen Küche) wird die Einstellgenauigkeit gröber mit der Größe der Gesamtzeit. So bleibt die Anzahl der notwendigen Umdrehungen des Drehknopfes auch für sehr große Intervalle überschaubar (Hallo Sauerteigbrotbäcker und Soßenspezialköche).

    Zeit   Genauikeit
    <30 s    1s
    <2 min   5s
    <5 min   10s
    <15 min  15 s
    <30 min  30 s
    <1 h     1 min
    <3 h     10 min
    <24 h    15 min
    <48 h    30 min
    >48h     1 h


### Beschleunigung bei schneller Drehung
Wird der Drehregler sehr schnell gedreht, verdoppelt oder vervierfacht sich der Wert für den jede Raststufe des Reglers steht. Dieses Verhalten wird durch geringeres Drehtempo wieder abgeschaltet, so dass nach einem groben Anwählen der Zielzeit eine exakte Positionierung im Rahmen der oben erklärten Genauigkeitsstufung möglich ist.

