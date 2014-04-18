function MattHelper()
{
	var MATT_POWER_PS  = ['&nbsp;','K','M','G','T','P'];
	var MATT_POWER_NPS = [' ','K','M','G','T','P'];

	/** Short helper to convert values to human readable format **/	
	this.humanReadableValue = function(value,unit,protectedSpace)
	{
		var power = 0;
		while (value >= 1000)
		{
			power++;
			value /= 1000;
		}

		var res;
		if (protectedSpace)
			res = value.toFixed(1) + " " + MATT_POWER_PS[power] + unit;
		else
			res = value.toFixed(1) + " " + MATT_POWER_NPS[power] + unit;

		return res;
	}
	
	/** Help to check values **/
	this.assert = function(condition,message)
	{
		if (!condition)
			throw Error("Assert failed" + (typeof message !== "undefined" ? ": " + message : ""));
	}
}

//setup global instance
var mattHelper = new MattHelper();
