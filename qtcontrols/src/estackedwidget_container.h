#ifndef ESTACKED_WIDGET_CONTAINER
#define ESTACKED_WIDGET_CONTAINER

#include <QWidget>
#include <QMap>

/** 
 * \brief A widget able to organize its overlapping children in a stack with a `z axis' priority
 *
 * A qtcontrols widget able to organize its children in a stack. Overlapping widgets
 * have a z axis that determines their priority on the stack. Each pain event on the EStackedWidgetContainer
 * produces the refresh of the child widgets according to their z axis priority.
 * An example is provided into <em>qtango/test/stackedWidgets</em> directory.
*/
class EStackedWidgetContainer : public QWidget
{
  Q_OBJECT
  public:
    /** \brief Constructs a container able to organize its overlapping children in a stack.
     *
     * Create a container or add it to your graphical interface with the designer.
     * Then drag and drop children inside it to have the paint events captured and the 
     * children organized in the stack according to their priority, defined with 
     * addWidget() or setWidgetPriority().
     *
     * @see addWidget()
     * @see setWidgetPriority()
     */
    EStackedWidgetContainer(QWidget *parent);
    
    /** \brief adds a widget to the stacked container
     *
     * If you want a widget to be placed into the container and organized in the stack, please call
     * this method passing the  pointer to the widget and its priority.
     * @param w the widget to add to the stack
     * @param z the widget priority, the `z axis'. Widgets with a higher z value are placed higher on
     *                    the stack and will be displayed in the foreground.
     */
    void addWidget(QWidget *w, int z);
    
    /** \brief changes the widget priority of a previously added widget
     *
     * Changes the widget priority of a previously added widget. 
     * @param w pointer to the widget whose priority has to be changed. The widget <strong>must have been previously added</strong>
     *                     via the addWidget() method.
     * @param z the  widget priority: `z axis'. 
     * @see addWidget()
     */
    void setWidgetPriority(QWidget *w, int z);
    
    /** \brief returns the priority of a widget
     * 
     * @return the widget priority
     * @param w the widget whose priority you want to retrieve
     */
    int widgetPriority(QWidget *w);
    
    
  protected:
    
    /** \brief paint event capture
     *
     * The default implementation receives paint events and repaints the child widgets according to their 
     * priority, set with addWidget() or setWidgetPriority(). Widgets with higher priority are stacked above 
     * widgets with lower priority.
     * You can reimplement this one in your subclass.
     */
    virtual void paintEvent(QPaintEvent *);
    
  private:
    QMap<QWidget *, int> d_widgetsMap;
    QList<QWidget *> d_orderedWidgets;
    
    void updateOrderedList();
};


#endif
