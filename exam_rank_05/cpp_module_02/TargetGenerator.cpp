/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TargetGenerator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:53:31 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:56:50 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TargetGenerator.hpp"
#include "ATarget.hpp"

#include <iostream>

//constructor
TargetGenerator::TargetGenerator(void)
{
}

//destructor
TargetGenerator::~TargetGenerator(void)
{
	std::list<ATarget*>::iterator it;	
	for (it = targets.begin(); it != targets.end() ; it++)
		delete(*it);
}

void TargetGenerator::learnTargetType(ATarget *new_target)
{
	std::list<ATarget*>::iterator it;	

	for (it = targets.begin(); it != targets.end() ; it++)
	{
		if ((*it)->getType() == new_target->getType())
			return ; //do nothing
	}
	targets.push_back(new_target->clone());
}

void TargetGenerator::forgetTargetType(std::string const &name)
{
	std::list<ATarget*>::iterator it;	

	for (it = targets.begin(); it != targets.end() ; it++)
	{
		if ((*it)->getType() == name)
		{
			delete(*it);
			targets.erase(it);
			break;
		}
	}
}

ATarget*
TargetGenerator::createTarget(std::string const &name) const
{
	std::list<ATarget*>::const_iterator it;	
	ATarget *ptr = NULL;

	for (it = targets.begin(); it != targets.end() ; it++)
	{
		if ((*it)->getType() == name)
			ptr = *it;
	}
	return (ptr);
}


