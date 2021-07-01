/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATarget.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:51:41 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 06:01:46 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ATarget.hpp"
#include "ASpell.hpp"

#include <iostream>

//constructor
ATarget::ATarget(void) {}

//constructor
ATarget::ATarget(std::string const &type) :
	type(type)
{ }

//destructor
ATarget::~ATarget(void)
{ }

//copy
ATarget::ATarget(const ATarget &src)
{
	*this = src;
}

//copy operator
ATarget& ATarget::operator=(const ATarget &src)
{
	type = src.type;
	return *this;	
}

std::string const &ATarget::getType(void) const
{ return this->type; }

void 
ATarget::getHitBySpell(const ASpell &aspell) const
{
	std::cout << this->type << " has been " << aspell.getEffects() << "!" <<std::endl;
}
