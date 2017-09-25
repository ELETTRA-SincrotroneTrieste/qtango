#include "text_browser.h"

TextBrowser::TextBrowser(QWidget *parent) : QTextBrowser(parent)
{
   setReadOnly(true);
   initHtml();
}


void TextBrowser::initHtml()
{
   QString html;
   html = "<h3>Stats Analyzer</h3><p>Click on a curve to watch details.</p> <p>Select the curves you want to be visible"
	 " clicking on the corresponding element on the legend below the plot. You can <em>show/hide</em> all the curves "
	 "at once by checking/unchecking the box <cite>Show/Hide All</cite>.<br/>You are also able to customize each curve "
	 "style by selecting the curve, its style and symbol from the <em>combo boxes</em> below and clicking on the <cite>Apply</cite> "
	 "push button.</p>"
	 "<h3>QTangoCore stats</h3><p>In the graph you'll see a trace of the <em>QTangoCore</em> objects lifetime. <em>Only readers</em> "
	 " are traced, and among the objects in the plot, there are also some special ones representing the <em>DeviceThread</em> lifetime "
	 " the <em>TUtil</em> and the <em>Global Refresh Timer</em> setup.</p>"
	 "<p><strong>Note</strong>: the <em>global refresh signal</em> is <cite>emitted</cite> by the TUtil and so it is traced "
	 "inside the <em>TUtil curve</em></p>"
	 "<h3>Readers lifetime</h3><p>Readers' lifetime is traced as follows:"
	 "<ul><li>setSource()</li><li>TAction creation</li><li>read_attribute 1</li><li>...</li><li>read_attribute N</li>"
	 "<li>unsetSource()</li><li>TAction destruction</li></ul>"
	 "Remember that TActions are <em>shared</em> among the same source points, so you might see <em>only one TAction creation"
	 " and destruction</em> for all the sources configured with the same point.</p>";
   setHtml(html);
}