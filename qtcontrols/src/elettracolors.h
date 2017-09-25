#ifndef ELETTRA_ECOLOR_H
#define ELETTRA_ECOLOR_H

#include <QColor>
#include <QDialog>
#include <QStringList>

#define EGREEN 		QColor(93, 234, 78)
#define EYELLOW  	QColor(247, 248, 156)
#define EGRAY  		QColor(209,207,204)
#define ECYAN 		QColor(185,222,219)
#define ERED   		QColor(250, 80, 80)
#define EBLUE  		QColor(100, 131, 201)
#define EORANGE		QColor(243, 186, 109)
#define EVIOLET 	QColor(200, 148, 208)
#define EDARK_GREEN	QColor(92, 151, 109)
#define EMAROON		QColor(157, 137, 137)
#define EPINK		QColor(231, 182, 226)
#define EFUCHSIA	QColor(240, 130, 228)
#define EBLACK		Qt::black
#define ELIGHT_GRAY	QColor(236, 232, 232)
#define EDARK_GRAY	QColor(155,155,155)
#define EGREENWATER	QColor(153, 208, 194)
#define EDIRTYYELLOW	QColor(187, 192, 44)
#define EBLUEGRAY	QColor(127, 135, 169)
#define BOOSTERCOLOR	QColor(205,230,255)
#define ELETTRACOLOR	QColor(220,255,222)
#define FERMICOLOR QColor(224,230,230)

/* KDE 4 palette */
#define KRED		QColor(231, 35, 0)
/* a red with saturation set to 90, slightly different from the
 * previous one: let's try it.
 */
#define KRED90SAT	QColor(231, 55, 23)
#define KGREEN		QColor(128, 195, 155)
#define KDARKGREEN	QColor(0, 151, 84)
#define KDARKRED	QColor(181, 0, 47)
#define KBLUE		QColor(0, 148, 213)
#define KDARKBLUE	QColor(0, 113, 188)
#define KVERYDARKBLUE	QColor(0, 65, 136)
#define KDARKCYAN	QColor(179, 221, 245)
#define KCYAN		QColor(197, 217, 240)
#define KMAGENTA	QColor(186, 144, 192)
#define KPINK		QColor(246, 207, 221)
#define KDARKPINK	QColor(240, 164, 185)
#define KVIOLET		QColor(209, 176, 210)
#define KDARKVIOLET	QColor(186, 144, 192)
#define KVERYDARKVIOLET QColor(155, 87, 159)
#define KYELLOW		QColor(255, 245, 146)
#define KDARKYELLOW	QColor(255, 233, 68)
#define KVERYDARKYELLOW QColor(245, 223, 58)
#define KORANGE		QColor(249, 186, 7)
#define KLIGHTGRAY	QColor(218, 221, 224)
#define KGRAY		QColor(187, 191, 195)
#define KDARKGRAY	QColor(157, 161, 166)
#define KALMOSTBLACK	QColor(109, 113, 121)
#define KCAMEL		QColor(243, 223, 198)
#define KMAROON		QColor(217, 174, 126)
#define KDARKMAROON	QColor(159, 95, 0)
#define KDARKWATER	QColor(164, 158, 205)
#define KPINKPIG	QColor(240, 164, 185)

#define KGRAYFERMIHIGHLIGHT 		QColor(154,160,160)
#define KGRAYFERMIALTERNATEBASE 	QColor(184,190,190)
#define KBOOSTERHIGHLIGHT		QColor(91,157,222)
#define KBOOSTERALTERNATEBASE		QColor(175,215,255)
#define KELETTRAHIGHLIGHT 		QColor(145,218,150)
#define KELETTRAALTERNATEBASE		QColor(180,205,182) 


namespace Elettra
{
	/* List of accepted colors 
	 * Check consistency with the char* colorlist 
	 * used in the .cpp!
	 */
	enum colors { white,  darkGray, gray, lightGray, red, darkRed, darkMaroon, maroon, camel,
		orange,  yellow,  dirtyYellow, darkYellow, green,  greenWater, darkGreen, 
		cyan, darkCyan, darkWater, blue, darkBlue, veryDarkBlue,
		blueGray, violet, darkViolet,
  		darkPink, pink, pinkPig,  fuchsia, almostBlack, black, boosterColor, elettraColor, fermiColor, elettraHighlight, 
 		elettraAlternateBase, boosterHighlight, boosterAlternateBase, fermiHighlight,
		/* let this color fermiAlternateBase be the LAST! */ fermiAlternateBase};
	
	/** From your program you can show the available palette calling 
	 * this method. 
	 * It automatically creates and shows the palette window.
	 */
	void showColorPalette();

}

/** This one returns the colour corresponding to the code
 * passed as argument.
 */
QColor EColor(int colorcode);

QColor EColor(const QString& colorString);

QPalette EPalette(QString);

/** Builds a dialog window that shows all the available colors
 * together with their name.
 * Note that this is not inside the Elettra namespace.
 */
class EColorSet : public QDialog
{
	public:
	/** Constructor: builds a Dialog with the Elettra palette.
	 * Accepts the parent widget as parameter.
	 */
	EColorSet(QWidget *parent);
	~EColorSet();

	private:
};


#endif

