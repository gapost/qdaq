#ifndef RTDATACHANNEL_H
#define RTDATACHANNEL_H

#include "QDaqJob.h"
#include "QDaqTypes.h"

#include "math_util.h"

namespace mu
{
	class Parser;
}

/** Objects that represent a stream of numerical data, a signal.

Real time data is handeled in RtLab through the QDaqChannel class.
An instrument provides measurement data through QDaqChannel objects.
On-line filtering and all types of data processing is also done with QDaqChannel and its
descendants.

\ingroup QDaq Core
*/
class RTLAB_BASE_EXPORT QDaqChannel : public QDaqJob
{
	Q_OBJECT

	/** User supplied name.
	Used when displaying the channel.
	*/
	Q_PROPERTY(QString signalName READ signalName WRITE setSignalName)
	/** User supplied unit of measurement.
	Used when displaying the channel.
	*/
	Q_PROPERTY(QString unit READ unit WRITE setUnit)
	/** Option for formatting channel data for textual representation.
	Used when displaying the channel.
	*/
	Q_PROPERTY(NumberFormat format READ format WRITE setFormat)
	/** # of decimal digits to display.
	Used when displaying the channel in fixed or scientific format.
	*/
	Q_PROPERTY(int digits READ digits WRITE setDigits)
	/** Signal range.
	When the signal is outside the range an error is raised.
	*/
    Q_PROPERTY(QDaqDoubleVector range READ range WRITE setRange)
	/** Channel offset.
	Applied imediately when data is inserted in the channel by the transformation y=a*x+b.
	b is the offset and a is the multiplier.
	*/
	Q_PROPERTY(double offset READ offset WRITE setOffset)
	/** Channel multiplier.
	Applied imediately when data is inserted in the channel by the transformation y=a*x+b.
	b is the offset and a is the multiplier.
	*/
	Q_PROPERTY(double multiplier READ multiplier WRITE setMultiplier)
	/** Type of on-line averaging.
	*/
	Q_PROPERTY(AveragingType averaging READ averaging WRITE setAveraging)
	/** Forgetting factor value.
	Only used when this type of averaging is active.
	*/
	Q_PROPERTY(double forgettingFactor READ forgettingFactor WRITE setForgettingFactor)
	/** Averaging depth.
	Number of past data values used in averaging.
	*/
	Q_PROPERTY(uint depth READ depth WRITE setDepth)
	/** Channel memory used.
	Number of values stored in internal channel memory.
	*/
	Q_PROPERTY(uint memsize READ memsize)
	/** True if valid data exist on the channel.
	If dataReady is true, then value() & std() return valid numbers.
	*/
	Q_PROPERTY(bool dataReady READ dataReady)
	/** muParser Expression.
	If set the expression is executed on the channel data.
	Note that the data goes first through muParser and then they are scaled with multiplier and offset.
	*/
	Q_PROPERTY(QString parserExpression READ parserExpression WRITE setParserExpression)

	Q_ENUMS(AveragingType)
	Q_ENUMS(NumberFormat)

public:
	/** Type of channel averaging.
	*/
	enum AveragingType {
		None, /**< No averaging. */
		Running, /**< Running (box) averaging. */
		Delta, /**< Running average for signals of alternating sign. */
		ForgettingFactor /**< Running average with forgetting (exponential weighting). */
	};

	/** Type of format for textual representation of channel data.
	*/
	enum NumberFormat {
		General, /**< No special formating. */
		FixedPoint, /**< No special formating. */
		Scientific, /**< No special formating. */
		Time /**< No special formating. */
	};

protected:
	QString signalName_, unit_;
    AveragingType type_;
    NumberFormat fmt_;
    int digits_;
	double v_, dv_, offset_, multiplier_;
    mu::Parser* parser_;
    bool dataReady_;
    QDaqDoubleVector range_;
    // a counter incremented at each new value
    uint counter_;
	uint depth_;
	double ff_, ffw_;

	// channel buffer
    math::circular_buffer<double> buff_;

	virtual bool arm_();
    virtual bool run();

	virtual void registerTypes(QScriptEngine* e);

	// do the averaging operations in the channel
	bool average();

public:
    Q_INVOKABLE explicit QDaqChannel(const QString& name);
    virtual ~QDaqChannel(void);

	virtual void detach();

	// getters
	QString signalName() const { return signalName_; }
	QString unit() const { return unit_; }
	NumberFormat format() const { return fmt_; }
	int digits() const { return digits_; }
    QDaqDoubleVector range() const { return range_; }
	AveragingType averaging() const { return type_; }
	double forgettingFactor() const { return ff_; }
	double offset() const { return offset_; }
	double multiplier() const { return multiplier_; }
	uint memsize() const { return buff_.capacity(); }
	uint depth() const { return depth_; }
	bool dataReady() const { return dataReady_; }
	QString parserExpression() const;

	// setters
	void setSignalName(QString v);
	void setUnit(QString v);
	void setFormat(NumberFormat v) { fmt_ = v; }
	void setDigits(int n) { digits_ = n; }
    void setRange(const QDaqDoubleVector& v);
	void setOffset(double v);
	void setMultiplier(double v);
	void setAveraging(AveragingType t);
	void setForgettingFactor(double v);
	void setDepth(uint d);
	void setParserExpression(const QString& s);


	void forceProcces();

	double last() const { return buff_.last(); }

	virtual QString formatedValue();

public slots:
	/** Insert a value into the channel. */
	void push(double v) { buff_ << v; counter_++; }
	/** Clear internal channel memory.*/
	void clear();
	/** Get the current channel value. */
	double value() const { return v_; }
	/** Get the current channel value standard deviation. */
	double std() const { return dv_; }
};

/** A channel for time measurement.

  \ingroup RtCore

  Every time the channel is executed it registers the current time.

  Can be used as a timer.

*/
class RTLAB_BASE_EXPORT QDaqTimeChannel : public QDaqChannel
{
    Q_OBJECT
protected:
    virtual bool run();
public:
    Q_INVOKABLE explicit QDaqTimeChannel(const QString& name);
    virtual ~QDaqTimeChannel(void);
};

/** A channel for testing.

  \ingroup RtCore

  The test channel generates data according to the TestType and parameter chosen.
  The data can be random noise, a continously increasing/decreasing value, or a sine wave.

*/
class RTLAB_BASE_EXPORT QDaqTestChannel : public QDaqChannel
{
    Q_OBJECT

    /// Set the test type.
    Q_PROPERTY(TestType type READ type WRITE setType)
    /** A parameter with different meaning according to type of test.
      For random noise it is the amplitude.
      For inc/dec it is the increment/decrement.
      For sine wave it is the frequency.
      */
    Q_PROPERTY(double par READ par WRITE setPar)

    Q_ENUMS(TestType)

public:
    enum TestType { Random, Inc, Dec, Sin };

    virtual void registerTypes(QScriptEngine *e);

protected:
    TestType type_;
    double par_;
    double v;

    virtual bool run();

public:
    Q_INVOKABLE explicit QDaqTestChannel(const QString& name);
    virtual ~QDaqTestChannel(void);


    TestType type() const { return type_; }
    void setType(TestType type) { type_ = type; v = 0; }
    double par() const { return par_; }
    void setPar(double v) { par_ = v; }
};

/** A channel for filtering another channel's data.

  \ingroup QDaqCore

  Performs filtering operations on data from the specified inputChannel.

  If the filter channel is the child of another data channel, then automatically
  it takes input from its parent (unless another inputChannel is specified).

*/
class RTLAB_BASE_EXPORT QDaqFilterChannel : public QDaqChannel
{
    Q_OBJECT


    /** The input channel to filter.
      */
    Q_PROPERTY(QDaqObject* inputChannel READ inputChannel WRITE setInputChannel)

protected:
    QPointer<QDaqChannel> inputChannel_;

    virtual bool run();

public:
    QDaqFilterChannel(const QString& name);

    void setInputChannel(QDaqObject* obj);
    QDaqObject* inputChannel();

};
#endif // QDAQDATACHANNEL_H
