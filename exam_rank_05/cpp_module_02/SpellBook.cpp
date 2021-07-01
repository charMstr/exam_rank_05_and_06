/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpellBook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:36:56 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:46:40 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SpellBook.hpp"
#include "ASpell.hpp"

#include <iostream>

//constructor
SpellBook::SpellBook(void)
{
}

//destructor
SpellBook::~SpellBook(void)
{
	std::list<ASpell*>::iterator it;	
	for (it = spells.begin(); it != spells.end() ; it++)
		delete(*it);
}

void SpellBook::learnSpell(ASpell *new_spell)
{
	std::list<ASpell*>::iterator it;	

	for (it = spells.begin(); it != spells.end() ; it++)
	{
		if ((*it)->getName() == new_spell->getName())
			return ; //do nothing
	}
	spells.push_back(new_spell->clone());
}

void SpellBook::forgetSpell(std::string const &name)
{
	std::list<ASpell*>::iterator it;	

	for (it = spells.begin(); it != spells.end() ; it++)
	{
		if ((*it)->getName() == name)
		{
			delete(*it);
			spells.erase(it);
			break;
		}
	}
}

ASpell*
SpellBook::createSpell(std::string const &name) const
{
	std::list<ASpell*>::const_iterator it;	
	ASpell *ptr = NULL;

	for (it = spells.begin(); it != spells.end() ; it++)
	{
		if ((*it)->getName() == name)
			ptr = *it;
	}
	return (ptr);
}

