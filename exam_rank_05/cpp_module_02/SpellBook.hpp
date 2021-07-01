/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpellBook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:33:49 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:50:18 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPELLBOOK_HPP
# define SPELLBOOK_HPP

#include <iostream>
#include <list>

class ASpell;

class SpellBook
{
	protected:
	//DATA
		std::list<ASpell*> spells;

	//FUNCTIONS:	
		SpellBook(const SpellBook &src);
		SpellBook &operator=(const SpellBook &src);
	public:	
	//FUNCTIONS:	
		//constructor
		SpellBook(void);
		//destructor
		virtual ~SpellBook(void);

		void learnSpell(ASpell *);
		void forgetSpell(std::string const &);
		ASpell* createSpell(std::string const &) const;
};

#endif
