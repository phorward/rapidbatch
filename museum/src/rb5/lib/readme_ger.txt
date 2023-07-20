J.M.K S.F. RapidBATCH 5.1.0 for 32-Bit Windows
INFORMATIONEN ZU DEN ERWEITERTEN BIBLIOTHEKEN
###########################################################

Dieses Verzeichnis enthält erweiterte Biblotheken, die in
RapidBATCH selbst implementiert wurden. Diese Bibliotheken
werden von vielen Scripts per INCLUDE-Befehl eingebunden,
damit die dort definierten Funktionen und Prozeduren zur
schnelleren Lösung von Problemen verwendet werden können.

Die erweiterten Bibliotheken gehören fest zum RapidBATCH
Paketumfang, und dürfen daher, wie in der Lizenzvereinba-
rung vorgegeben, ebenso wenig ohne das RapidBATCH Programm-
paket vertrieben werden wie das RapidBATCH-Softwarepaket
selber.

Sie dürfen diese Biblotehken jedoch frei in Ihre Scripts
einbinden (und mit Ihren Scripts zusammen compilieren und
verteilen) und ZU EIGENEN ZWECKEN modifizieren.
Sollten Sie eine neue, nützliche Funktion oder gar eine
ganze Bibliothek selbst geschrieben haben, so kann diese
evtl. in ein späteres Release von RapidBATCH mit aufge-
nommen und anderen Programmierern bereitgestellt werden.

Genaue Informationen zu den einzelnen Funktionen und Pro-
zeduren der jeweiligen Biblothek finden Sie direkt in Bi-
bliotheks-Quellcode als Kommentare.

In diesem Releasepaket sind folgende Bibliotheken ent-
halten:


+ ARRAY.RB	Funktionalitäten zur Verarbeitung von
		Arrays.

+ DATABASE.RB	Implementation einer einfachen Datenbank
		Engine, welche Datensätze in Textdateien
		ablegt.
		Diese Bibliothek befindet sich noch in
		der Erprobungsphase und kann Fehler ent-
		halten.

+ DATE.RB	Datumsberechnungsfunktionalitäten.

+ DEFAULT.RB	Keine Library, sondern eine Default-Include
		Datei, die automatisch bei jedem Aufruf des
		RapidBATCH Interpreters eingebunden wird;
		Hier können weitere Librarys standardmäßig
		in jedes Script, welches Sie erstellen oder
		ausführen, eingebunden werden.

+ DIALOG.RB	Hilfreiche Funktionalitäten die in Verbin-
		dung mit benutzerdefinierten Dialogfenstern
		sehr nützlich sind; Ausserdem sind hier
		universell einsetzbare, weitere Dialoge,
		die in RapidBATCH ähnlich wie LISTBOX oder
		INPUTBOX verwendet werden können, definiert.

+ MATH.RB	Mathematische Funktionalitäten und Berech-
		nungen.

+ STRING.RB	Erweiterte und vereinfachte Stringverarbei-
		tungsfunktionaltitäten.

+ SYSTEM.RB	Ergänzende system-spezifische Funktionali-
		täten.

--
J.M.K S.F. RapidBATCH v5.1.0 (32-Bit) - Professional Edition
Copyright © 2000-2006 by J.M.K S.F., Jan Max Meyer