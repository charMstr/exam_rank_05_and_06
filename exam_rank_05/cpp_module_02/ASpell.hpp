/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASpell.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:42:59 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:01:07 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASPELL_HPP
# define ASPELL_HPP

#include <string>

class ATarget;

class ASpell
{
	protected:
	//DATA
		std::string name;
		std::string effects;

	public:	
	//FUNCTIONS:	
		//constructor
		ASpell(void);
		ASpell(const ASpell &src);
		ASpell &operator=(const ASpell &src);

		//constructor
		ASpell(std::string const &name, std::string const &effects);
		//destructor
		virtual ~ASpell(void);

		std::string const &getName(void) const;
		std::string const &getEffects(void) const;

		void launch(const ATarget &) const;

	virtual ASpell* clone(void) const = 0;
};

#endif
