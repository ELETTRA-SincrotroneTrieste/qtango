#ifndef EPLOT_CONFIGURATION_WIDGET_H
#define EPLOT_CONFIGURATION_WIDGET_H

#include <QDialog>
#include <QSet>
#include "eplotlight.h"

#include "ui_configuration.h"

class EPlotCurve;

class EPlotConfigurationWidget : public QWidget
{
  Q_OBJECT
  public:
	
	EPlotConfigurationWidget(QWidget *parent);

  signals:
	void xAxisAutoscaleEnabled(bool );
	void yAxisAutoscaleEnabled(bool );
	
	
	
	void yUpperBound(double);
	void xUpperBound(double);
	
	void xLowerBound(double);
	void yLowerBound(double);

	void xAutoscaleAdjustEnabled(bool);
	void yAutoscaleAdjustEnabled(bool);
	
	void yAutoscaleAdjustment(double);
	void xAutoscaleAdjustment(double);
	
        void curvesStyle(EPlotLightBase::CurveStyle );
	
  private:
	Ui::EPlotConfigurationWidget ui;
	EPlotLight *d_plot;
	QSet<QString> d_changedProperties;
	
	void init();
	void initAppearenceSettings();
	int getIndexOfProperty(const QString& name);
	double getPropertyAsDouble(const QString& name);
	void connectAppearenceWidgets();
	
 
  protected slots:
	void xAdjustmentChanged();
	void yAdjustmentChanged();
	void curveStyleChanged();
	void curveSelectionChanged(int);
	void writeProperty(const QString& name, const QVariant &val);
	void editCurveColor();
	void propertyChanged();
	void saveStyles();
	void apply();
};

/** \brief the EPlotLight configuration dialog.
 *
 * The parent used in the constructor must be a pointer to 
 * an EPlotLight object.
 */
class EPlotConfigurationDialog : public QDialog
{
  Q_OBJECT
  public:
	EPlotConfigurationDialog(QWidget *parent);
};

#endif
