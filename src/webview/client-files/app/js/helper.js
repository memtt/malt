/**********************************************************************/
function MaltHelper()
{
	/********************************************************************/
	this.MALT_POWER_PS  = ['&nbsp;','K','M','G','T','P'];
	this.MALT_POWER_NPS = [' ','K','M','G','T','P'];
	this.MALT_SUBPOWER_PS  = ['&nbsp;','m','u','n'];
	this.MALT_SUBPOWER_NPS = [' ','m','u','n','p'];
}

/**********************************************************************/
/**
 * Help to padd interger value with 0
 * @param value Define the value to padd
 * @param size Define the final size of the string.
**/
MaltHelper.prototype.zeroFill = function(value,size)
{
	var cur = value;
	var pad = '';
	
	//padd
	while (size > 1)
	{
		cur = cur / 10;
		size--;
		if (cur < 1)
			pad+='0';
	}
	
	//final string
	var res = pad + value;

	//errors
	if (res.length != size)
		console.log('Invalid size of padded string !');

	//return
	return res;
}

/**********************************************************************/
/**
 * Help to convert ticks timings to hh:mm:ss.
 * @param t time to convert
 * @param ticksPerSec Ref to convert ticks to seconds.
**/
MaltHelper.prototype.ticksToHourMinSec = function(t,ticksPerSec)
{
	//calc sec
	var tot = Math.round(t / ticksPerSec);
	
	//calc min
	var sec = tot % 60;
	var min = Math.floor(tot / 60);
	var hour = Math.floor(tot / 60 / 60);
	var days = Math.floor(tot / 60 / 60 / 24);
	var msec = Math.round(100*((t / ticksPerSec) - tot));

	//gen string
	var res = '';
	if (tot >= 60*60*24) res += this.zeroFill(days,2) + '-';
	res += this.zeroFill(hour,2) + ':';
	res += this.zeroFill(min,2) + ':';
	res += this.zeroFill(sec,2) + '.';
	res += this.zeroFill(msec,2);

	return res;
}

/********************************************************************/
/** Short helper to convert values to human readable format **/	
MaltHelper.prototype.humanReadable = function(value,decimals,unit,protectedSpace)
{
	var mul = 1000;
	if (unit == 'B' || unit == 'B')
		mul = 1024;
	
	if (value >= 1 && value < mul)
		decimals = 0;
	
	if (value == 0)
		decimals = 0;
	
	if (value >= 0.1 || value == 0)
	{
		var power = 0;
		while (value >= mul)
		{
			power++;
			value /= mul;
		}

		var res;
		if (protectedSpace)
			res = value.toFixed(decimals) + " " + this.MALT_POWER_PS[power] + unit;
		else
			res = value.toFixed(decimals) + " " + this.MALT_POWER_NPS[power] + unit;
	} else {
		var power = 0;
		while (value < 0.1 && power < 4)
		{
			power++;
			value *= 1000.0;
		}

		var res;
		if (protectedSpace)
			res = value.toFixed(decimals) + " " + this.MALT_SUBPOWER_PS[power] + unit;
		else
			res = value.toFixed(decimals) + " " + this.MALT_SUBPOWER_NPS[power] + unit;
	}

	return res;
}

/********************************************************************/

/********************************************************************/
MaltHelper.prototype.humanReadableTimes = function(value)
{
	if (value > 0 && value < 1)
	{
		var tmp = value;
		var exp = 1;
		while (tmp < 1)
		{
			tmp*=10;
			exp++;
		}
		return value.toFixed(exp);
	}
	
	days=Math.floor(value/(60*60*24));
	hours=Math.floor(value/(60*60)) % 24;
	minutes=Math.floor(value/60) % 60;
	seconds=value % 60;
	
	var res = "";
	if (days > 0)
		res+=days+".";
	if (hours > 0 || days > 0)
		res+=String("00"+hours).slice(-2)+":";
	if (hours > 0 || days > 0 || minutes > 0)
		res+=String("00"+minutes).slice(-2)+":";
	if (hours > 0 || days > 0 || minutes > 0 || seconds > 0)
		res+=seconds.toFixed(1);
	return res;
}

/********************************************************************/
MaltHelper.prototype.mergeStackMinMaxInfo = function(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/********************************************************************/
MaltHelper.prototype.mergeStackInfoDatas = function(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	onto.globalPeak += value.globalPeak;

	this.mergeStackMinMaxInfo(onto.alloc,value.alloc);
	this.mergeStackMinMaxInfo(onto.free,value.free);
	this.mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/**********************************************************************/

// If this is ran on server-side, we export the class as a module
// Otherwise, we create a globally shared object for this class
if(typeof exports === 'undefined') {
	maltHelper = new MaltHelper();
} else {
	module.exports = MaltHelper;
}
