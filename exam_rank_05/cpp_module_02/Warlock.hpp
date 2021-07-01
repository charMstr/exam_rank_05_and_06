/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Warlock.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:26:51 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:45:48 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WARLOCK_HPP
# define WARLOCK_HPP

#include <string>
#include <list>
#include  "SpellBook.hpp"

class ATarget;
class ASpell;

class Warlock
{
	private:
	//DATA
		std::string name;
		std::string title;
		SpellBook spell_book;

	//FUNCTIONS:	
		//constructor
		Warlock(void);
		Warlock(const Warlock &src);
		Warlock &operator=(const Warlock &src);
	
	public:	
		//constructor
		Warlock(std::string const &name, std::string const &title);
		//destructor
		virtual ~Warlock(void);

		std::string const &getName(void) const;
		std::string const &getTitle(void) const;
		void	setTitle(std::string const &title);
		void introduce() const;

		void learnSpell(ASpell *);
		void forgetSpell(std::string const &);
		void launchSpell(std::string const &, const ATarget &);
};

#endif
