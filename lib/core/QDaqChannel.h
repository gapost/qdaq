#ifndef RTDATACHANNEL_H
#define RTDATACHANNEL_H

#include "QDaqJob.h"
#include "QDaqTypes.h"

#include "math_util.h"
#include <vector>

namespace mu
{
	class Parser;
}

/**
 * @brief A class that represent a stream of numerical data, a signal.
 *
 * @ingroup Core
 * @ingroup ScriptAPI
 *
 * Real time data is handeled in QSaq through the QDaqChannel class.
 * E.g., an instrument provides measurement data through QDaqChannel objects.
 *
 * QDaqChannel provides tools for displaying, transforming, averaging the data
 * of the channel.
 *
 */
class QDAQ_EXPORT QDaqChannel : public QDaqJob
{
	Q_OBJECT

    /// Type of the channel.  
    Q_PROPERTY(ChannelType type READ type WRITE setType)
    /** User supplied signal name.
    Used when displaying the channel.
    It is different from the object name.
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
    /** Number of decimal digits to display.
	Used when displaying the channel in fixed or scientific format.
	*/
	Q_PROPERTY(int digits READ digits WRITE setDigits)
	/** Signal range.
	When the signal is outside the range an error is raised.
	*/
    Q_PROPERTY(QDaqVector range READ range WRITE setRange)
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

public:
    /** Type of the channel.
    */
    enum ChannelType {
        Normal,  /**< Normal channel - nothing special. */
        Clock,    /**< Records time in each repetition. Can be used for time measurement. */
        Random,  /**< Generates random samples. */
        Inc,     /**< Starting from an initial value increments by 1 in each repetition. */
        Dec      /**< Starting from an initial value decrements by 1 in each repetition. */
    };
    Q_ENUM(ChannelType)

	/** Type of channel averaging.
	*/
	enum AveragingType {
		None, /**< No averaging. */
		Running, /**< Running (box) averaging. */
		Delta, /**< Running average for signals of alternating sign. */
        ForgettingFactor, /**< Running average with forgetting (exponential weighting). */
        Median /**< Running median filter. */
    };
    Q_ENUM(AveragingType)

	/** Type of format for textual representation of channel data.
	*/
	enum NumberFormat {
		General, /**< No special formating. */
		FixedPoint, /**< No special formating. */
		Scientific, /**< No special formating. */
		Time /**< No special formating. */
	};
    Q_ENUM(NumberFormat)

protected:
    ChannelType channeltype_;
	QString signalName_, unit_;
    AveragingType type_;
    NumberFormat fmt_;
    int digits_;
	double v_, dv_, offset_, multiplier_;
    mu::Parser* parser_;
    bool dataReady_;
    QDaqVector range_;
    // a counter incremented at each new value
    uint counter_;
	uint depth_;
	double ff_, ffw_;

	// channel buffer
    math::circular_buffer<double> buff_;

    //buffer used for median
    std::vector<double> sorted_buffer;


	virtual bool arm_();

    /**
     * @brief Perform channel tasks.
     *
     * The tasks are performed in the following order:
     *
     *   - If channel is of a special type, then its function is performed.
     *     E.g. if the type is Random a random number is generated.
     *   - averaging is performed
     *   - the mean value goes through muParser
     *   - the mean value is scaled and shifted (multiplier*v + offset)
     *   - the mean value is checked for under/over range
     *
     * If new data is available the updateWidgets signal is emitted.
     *
     * @return always return true.
     */
    virtual bool run();

	// do the averaging operations in the channel
	bool average();

public:
    Q_INVOKABLE explicit QDaqChannel(const QString& name);
    virtual ~QDaqChannel(void);

	virtual void detach();

	// getters
    ChannelType type() const { return channeltype_; }
	QString signalName() const { return signalName_; }
	QString unit() const { return unit_; }
	NumberFormat format() const { return fmt_; }
	int digits() const { return digits_; }
    QDaqVector range() const { return range_; }
	AveragingType averaging() const { return type_; }
	double forgettingFactor() const { return ff_; }
	double offset() const { return offset_; }
	double multiplier() const { return multiplier_; }
	uint memsize() const { return buff_.capacity(); }
	uint depth() const { return depth_; }
	bool dataReady() const { return dataReady_; }
	QString parserExpression() const;

	// setters
    void setType(ChannelType t);
	void setSignalName(QString v);
	void setUnit(QString v);
	void setFormat(NumberFormat v) { fmt_ = v; }
	void setDigits(int n) { digits_ = n; }
    void setRange(const QDaqVector& v);
	void setOffset(double v);
	void setMultiplier(double v);
	void setAveraging(AveragingType t);
	void setForgettingFactor(double v);
	void setDepth(uint d);
	void setParserExpression(const QString& s);


	void forceProcces();

	double last() const { return buff_.last(); }

    /// Returns the channel value formatted according to format/digits
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

#endif // QDAQDATACHANNEL_H
