/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASpell.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:46:41 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:02:24 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASpell.hpp"
#include "ATarget.hpp"

#include <iostream>

//constructor
ASpell::ASpell(void) {}

//constructor
ASpell::ASpell(std::string const &name, std::string const &effects) :
	name(name),
	effects(effects)
{ }

//destructor
ASpell::~ASpell(void)
{ }

//copy
ASpell::ASpell(const ASpell &src)
{
	*this = src;
}

//copy operator
ASpell& ASpell::operator=(const ASpell &src)
{
	name = src.name;
	effects = src.effects;
	return *this;	
}

std::string const &ASpell::getName(void) const
{ return this->name; }

std::string const &ASpell::getEffects(void) const
{ return this->effects; }


void 
ASpell::launch(const ATarget &target) const
{
	target.getHitBySpell(*this);
}
