/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TargetGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:49:14 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:53:14 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TARGETGENERATOR_HPP
# define TARGETGENERATOR_HPP

#include <iostream>
#include <list>

class ATarget;

class TargetGenerator
{
	protected:
	//DATA
		std::list<ATarget*> targets;

	//FUNCTIONS:	
		//constructor
		TargetGenerator(const TargetGenerator &src);
		TargetGenerator &operator=(const TargetGenerator &src);

	public:	
	//FUNCTIONS:	
		//constructor
		TargetGenerator(void);
		//destructor
		virtual ~TargetGenerator(void);

		void learnTargetType(ATarget *);
		void forgetTargetType(std::string const &);
		ATarget* createTarget(std::string const &) const;
};

#endif
