J.M.K S.F. RapidBATCH 5.1.0 for 32-Bit Windows
INFORMATIONEN ZU DEN ERWEITERTEN BIBLIOTHEKEN
###########################################################

Dieses Verzeichnis enth�lt erweiterte Biblotheken, die in
RapidBATCH selbst implementiert wurden. Diese Bibliotheken
werden von vielen Scripts per INCLUDE-Befehl eingebunden,
damit die dort definierten Funktionen und Prozeduren zur
schnelleren L�sung von Problemen verwendet werden k�nnen.

Die erweiterten Bibliotheken geh�ren fest zum RapidBATCH
Paketumfang, und d�rfen daher, wie in der Lizenzvereinba-
rung vorgegeben, ebenso wenig ohne das RapidBATCH Programm-
paket vertrieben werden wie das RapidBATCH-Softwarepaket
selber.

Sie d�rfen diese Biblotehken jedoch frei in Ihre Scripts
einbinden (und mit Ihren Scripts zusammen compilieren und
verteilen) und ZU EIGENEN ZWECKEN modifizieren.
Sollten Sie eine neue, n�tzliche Funktion oder gar eine
ganze Bibliothek selbst geschrieben haben, so kann diese
evtl. in ein sp�teres Release von RapidBATCH mit aufge-
nommen und anderen Programmierern bereitgestellt werden.

Genaue Informationen zu den einzelnen Funktionen und Pro-
zeduren der jeweiligen Biblothek finden Sie direkt in Bi-
bliotheks-Quellcode als Kommentare.

In diesem Releasepaket sind folgende Bibliotheken ent-
halten:


+ ARRAY.RB	Funktionalit�ten zur Verarbeitung von
		Arrays.

+ DATABASE.RB	Implementation einer einfachen Datenbank
		Engine, welche Datens�tze in Textdateien
		ablegt.
		Diese Bibliothek befindet sich noch in
		der Erprobungsphase und kann Fehler ent-
		halten.

+ DATE.RB	Datumsberechnungsfunktionalit�ten.

+ DEFAULT.RB	Keine Library, sondern eine Default-Include
		Datei, die automatisch bei jedem Aufruf des
		RapidBATCH Interpreters eingebunden wird;
		Hier k�nnen weitere Librarys standardm��ig
		in jedes Script, welches Sie erstellen oder
		ausf�hren, eingebunden werden.

+ DIALOG.RB	Hilfreiche Funktionalit�ten die in Verbin-
		dung mit benutzerdefinierten Dialogfenstern
		sehr n�tzlich sind; Ausserdem sind hier
		universell einsetzbare, weitere Dialoge,
		die in RapidBATCH �hnlich wie LISTBOX oder
		INPUTBOX verwendet werden k�nnen, definiert.

+ MATH.RB	Mathematische Funktionalit�ten und Berech-
		nungen.

+ STRING.RB	Erweiterte und vereinfachte Stringverarbei-
		tungsfunktionaltit�ten.

+ SYSTEM.RB	Erg�nzende system-spezifische Funktionali-
		t�ten.

--
J.M.K S.F. RapidBATCH v5.1.0 (32-Bit) - Professional Edition
Copyright � 2000-2006 by J.M.K S.F., Jan Max Meyer