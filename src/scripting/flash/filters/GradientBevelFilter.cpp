/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2012-2013  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "scripting/flash/filters/GradientBevelFilter.h"
#include "scripting/class.h"
#include "scripting/argconv.h"
#include "scripting/flash/display/BitmapData.h"
#include "backends/rendering.h"
#include "scripting/toplevel/Array.h"


using namespace std;
using namespace lightspark;


GradientBevelFilter::GradientBevelFilter(ASWorker* wrk,Class_base* c):
	BitmapFilter(wrk,c,SUBTYPE_GRADIENTBEVELFILTER),
	angle(45),
	blurX(4.0),
	blurY(4.0),
	distance(4.0),
	knockout(false),
	quality(1),
	strength(1),
	type("inner")
{
}
GradientBevelFilter::GradientBevelFilter(ASWorker* wrk,Class_base* c, const GRADIENTBEVELFILTER& filter):
	BitmapFilter(wrk,c,SUBTYPE_GRADIENTBEVELFILTER),
	angle(filter.Angle),
	blurX(filter.BlurX),
	blurY(filter.BlurY),
	distance(filter.Distance),
	knockout(filter.Knockout),
	quality(filter.Passes),
	strength(filter.Strength),
	type(filter.InnerShadow ? "inner" : "outer") // TODO: is type set based on "onTop" ?
{
	if (filter.OnTop)
		LOG(LOG_NOT_IMPLEMENTED,"GradientBevelFilter onTop flag");
	if (filter.NumColors)
	{
		colors = _MR(Class<Array>::getInstanceSNoArgs(wrk));
		alphas = _MR(Class<Array>::getInstanceSNoArgs(wrk));
		for (uint8_t i = 0; i < filter.NumColors; i++)
		{
			colors->push(asAtomHandler::fromUInt(RGB(filter.GradientColors[i].Red,filter.GradientColors[i].Green,filter.GradientColors[i].Blue).toUInt()));
			alphas->push(asAtomHandler::fromNumber(wrk,filter.GradientColors[i].af(),false));
		}
	}
	if (filter.NumColors)
	{
		ratios = _MR(Class<Array>::getInstanceSNoArgs(wrk));
		for (uint8_t i = 0; i < filter.NumColors; i++)
		{
			ratios->push(asAtomHandler::fromUInt(filter.GradientRatio[i]));
		}
	}
}


void GradientBevelFilter::sinit(Class_base* c)
{
	CLASS_SETUP(c, BitmapFilter, _constructor, CLASS_SEALED | CLASS_FINAL);
	REGISTER_GETTER_SETTER(c,alphas);
	REGISTER_GETTER_SETTER(c,angle);
	REGISTER_GETTER_SETTER(c,blurX);
	REGISTER_GETTER_SETTER(c,blurY);
	REGISTER_GETTER_SETTER(c,colors);
	REGISTER_GETTER_SETTER(c,distance);
	REGISTER_GETTER_SETTER(c,knockout);
	REGISTER_GETTER_SETTER(c,quality);
	REGISTER_GETTER_SETTER(c,ratios);
	REGISTER_GETTER_SETTER(c,strength);
	REGISTER_GETTER_SETTER(c,type);
}

ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,alphas)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,angle)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,blurX)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,blurY)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,colors)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,distance)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,knockout)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,quality)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,ratios)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,strength)
ASFUNCTIONBODY_GETTER_SETTER_NOT_IMPLEMENTED(GradientBevelFilter,type)

void GradientBevelFilter::applyFilter(BitmapContainer* target, BitmapContainer* source, const RECT& sourceRect, number_t xpos, number_t ypos, number_t scalex, number_t scaley, DisplayObject* owner)
{
	if (type=="full")
		LOG(LOG_NOT_IMPLEMENTED,"GradientBevelFilter type 'full'");
	uint8_t* tmpdata = nullptr;
	if (source)
		tmpdata = source->getRectangleData(sourceRect);
	else
		tmpdata = target->getRectangleData(sourceRect);

	number_t gradientalphas[256];
	uint32_t gradientcolors[256];
	fillGradientColors(gradientalphas,gradientcolors,this->ratios.getPtr(), this->alphas.getPtr(), this->colors.getPtr());
	applyBlur(tmpdata,sourceRect.Xmax-sourceRect.Xmin,sourceRect.Ymax-sourceRect.Ymin,blurX,blurY,quality);

	applyBevelFilter(target,sourceRect,tmpdata,owner,distance,strength,type=="inner",knockout,gradientcolors,gradientalphas,angle,xpos,ypos,scalex,scaley);
	delete[] tmpdata;
}

void GradientBevelFilter::prepareShutdown()
{
	if (preparedforshutdown)
		return;
	BitmapFilter::prepareShutdown();
	if (colors)
		colors->prepareShutdown();
	if (alphas)
		alphas->prepareShutdown();
	if (ratios)
		ratios->prepareShutdown();
}

ASFUNCTIONBODY_ATOM(GradientBevelFilter,_constructor)
{
	GradientBevelFilter *th = asAtomHandler::as<GradientBevelFilter>(obj);
	ARG_CHECK(ARG_UNPACK(th->distance,4.0)(th->angle,45)(th->colors,NullRef)(th->alphas,NullRef)(th->ratios,NullRef)(th->blurX,4.0)(th->blurY,4.0)(th->strength,1)(th->quality,1)(th->type,"inner")(th->knockout,false));
}

bool GradientBevelFilter::compareFILTER(const FILTER& filter) const
{
	if (filter.GradientBevelFilter.OnTop)
		LOG(LOG_NOT_IMPLEMENTED,"GradientBevelFilter onTop flag");
	bool ret = filter.FilterID == FILTER::FILTER_GRADIENTBEVEL
		   && filter.GradientBevelFilter.Angle == this->angle
		   && filter.GradientBevelFilter.BlurX == this->blurX
		   && filter.GradientBevelFilter.BlurY == this->blurY
		   && filter.GradientBevelFilter.Distance == this->distance
		   && filter.GradientBevelFilter.Knockout == this->knockout
		   && filter.GradientBevelFilter.Passes == this->quality
		   && filter.GradientBevelFilter.Strength == this->strength
		   && this->type == (filter.GradientBevelFilter.InnerShadow ? "inner" : "outer");
	if (ret)
	{
		if (colors.isNull() || colors->size()!=filter.GradientBevelFilter.NumColors
			|| alphas.isNull() || alphas->size()!=filter.GradientBevelFilter.NumColors
			|| ratios.isNull() || ratios->size()!=filter.GradientBevelFilter.NumColors)
			ret = false;
	}
	if (ret)
	{
		if (filter.GradientBevelFilter.NumColors)
		{
			for (uint32_t  i = 0; i < filter.GradientBevelFilter.NumColors; i++)
			{
				asAtom a = asAtomHandler::invalidAtom;
				colors->at_nocheck(a,i);
				if (RGB(filter.GradientBevelFilter.GradientColors[i].Red,filter.GradientBevelFilter.GradientColors[i].Green,filter.GradientBevelFilter.GradientColors[i].Blue).toUInt() != asAtomHandler::toUInt(a))
				{
					ret = false;
					break;
				}
				alphas->at_nocheck(a,i);
				if (filter.GradientBevelFilter.GradientColors[i].af() != asAtomHandler::toNumber(a))
				{
					ret = false;
					break;
				}
			}
		}
	}
	if (ret)
	{
		if (filter.GradientBevelFilter.NumColors)
		{
			for (uint32_t  i = 0; i < filter.GradientBevelFilter.NumColors; i++)
			{
				asAtom a = asAtomHandler::invalidAtom;
				ratios->at_nocheck(a,i);
				if (filter.GradientBevelFilter.GradientRatio[i] != asAtomHandler::toUInt(a))
				{
					ret = false;
					break;
				}
			}
		}
	}
	return ret;
}
void GradientBevelFilter::getRenderFilterArgs(uint32_t step,float* args) const
{
	if (type=="full")
		LOG(LOG_NOT_IMPLEMENTED,"GradientBevelFilter type 'full'");
	uint32_t nextstep;
	if (getRenderFilterArgsBlur(args,blurX,blurY,step,quality,nextstep))
		return;
	else if (step == nextstep)
		getRenderFilterArgsBevel(args,type=="inner",knockout,strength,distance,angle);
	else
		args[0]=0.0;
}

void GradientBevelFilter::getRenderFilterGradientColors(float* gradientcolors) const
{
	number_t tmpgradientalphas[256];
	uint32_t tmpgradientcolors[256];
	fillGradientColors(tmpgradientalphas,tmpgradientcolors,this->ratios.getPtr(), this->alphas.getPtr(), this->colors.getPtr());
	for (uint32_t i = 0; i < 256; i++)
	{
		RGBA c(tmpgradientcolors[i],0);
		gradientcolors[i*4  ] = c.rf();
		gradientcolors[i*4+1] = c.gf();
		gradientcolors[i*4+2] = c.bf();
		gradientcolors[i*4+3] = tmpgradientalphas[i];
	}
}

BitmapFilter* GradientBevelFilter::cloneImpl() const
{
	GradientBevelFilter* cloned = Class<GradientBevelFilter>::getInstanceS(getInstanceWorker());
	cloned->alphas = alphas;
	cloned->angle = angle;
	cloned->blurX = blurX;
	cloned->blurY = blurY;
	cloned->colors = colors;
	cloned->distance = distance;
	cloned->knockout = knockout;
	cloned->quality = quality;
	cloned->ratios = ratios;
	cloned->strength = strength;
	cloned->type = type;
	return cloned;
}
