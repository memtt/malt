/********************************************************************/
var MATT_POWER_PS  = ['&nbsp;','K','M','G','T','P'];
var MATT_POWER_NPS = [' ','K','M','G','T','P'];

/********************************************************************/
function MattHelper()
{
}

/********************************************************************/
/** Short helper to convert values to human readable format **/	
MattHelper.prototype.humanReadableValue = function(value,unit,protectedSpace)
{
	var power = 0;
	while (value >= 1024)
	{
		power++;
		value /= 1024;
	}

	var res;
	if (protectedSpace)
		res = value.toFixed(1) + " " + MATT_POWER_PS[power] + unit;
	else
		res = value.toFixed(1) + " " + MATT_POWER_NPS[power] + unit;

	return res;
}

/********************************************************************/
/** Help to check values **/
MattHelper.prototype.assert = function(condition,message)
{
	if (!condition)
		throw Error("Assert failed" + (typeof message !== "undefined" ? ": " + message : ""));
}

/********************************************************************/
MattHelper.prototype.mergeStackMinMaxInfo = function(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/********************************************************************/
MattHelper.prototype.mergeStackInfoDatas = function(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;

	this.mergeStackMinMaxInfo(onto.alloc,value.alloc);
	this.mergeStackMinMaxInfo(onto.free,value.free);
	this.mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/********************************************************************/
//setup global instance
var mattHelper = new MattHelper();
